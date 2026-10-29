#include <stdio.h>
#include <dolphin.h>
#include <dolphin/os.h>

// internal include
#include "__os.h"

/* Set in SRR1 by Gekko when a floating-point exception raised the program
 * exception. */
#define SRR1_FP_EXCEPTION 0x00100000

/* FPSCR exception-status bits the OS clears when it hands the FPU state to a
 * handler. */
#define FPSCR_STATUS_MASK 0x6005f8ff

OSErrorHandler __OSErrorTable[__OS_ERROR_MAX];

/* FPSCR exception-enable bits the OS applies to every thread: invalid
 * operation, overflow, underflow, zero divide and inexact. */
u32 __OSFpscrEnableBits = FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE;

extern OSContext* __OSFPUContext : (OS_BASE_CACHED | 0x00D8);

void OSReport(char* msg, ...) {
  va_list marker;
  va_start(marker, msg);
  vprintf(msg, marker);
  va_end(marker);
}

// UNUSED: the game never calls this; dead-stripped at link.
static void OSPanic(char* file, int line, char* msg, ...) {
  va_list marker;
  u32 i;
  u32* p;

  OSDisableInterrupts();
  va_start(marker, msg);
  vprintf(msg, marker);
  va_end(marker);
  OSReport(" in \"%s\" on line %d.\n", file, line);

  OSReport("\nAddress:      Back Chain    LR Save\n");
  for (i = 0, p = (u32*)OSGetStackPointer(); p && (u32)p != 0xffffffff && i++ < 16; p = (u32*)*p) {
    OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
  }

  PPCHalt();
}

OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler) {
    OSErrorHandler oldHandler;
    BOOL enabled;
    OSThread* thread;
    u32 msr;
    u32 fpscr;
    u32 i;

    enabled = OSDisableInterrupts();
    oldHandler = __OSErrorTable[error];
    __OSErrorTable[error] = handler;

    if (error == __OS_ERROR_FPU_EXCEPTION) {
        msr = PPCMfmsr();
        PPCMtmsr(msr | MSR_FP);
        fpscr = PPCMffpscr();
        if (handler) {
            for (thread = __OSActiveThreadQueue.head; thread; thread = thread->linkActive.next) {
                thread->context.srr1 |= MSR_FE0 | MSR_FE1;
                if (!(thread->context.state & OS_CONTEXT_STATE_FPSAVED)) {
                    thread->context.state |= OS_CONTEXT_STATE_FPSAVED;
                    for (i = 0; i < 32; i++) {
                        *(u64*)&thread->context.fpr[i] = 0xffffffffffffffff;
                        *(u64*)&thread->context.psf[i] = 0xffffffffffffffff;
                    }
                    thread->context.fpscr = FPSCR_NI;
                }
                thread->context.fpscr |= __OSFpscrEnableBits & (FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE);
                thread->context.fpscr &= FPSCR_STATUS_MASK;
            }
            fpscr |= __OSFpscrEnableBits & (FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE);
            msr |= MSR_FE0 | MSR_FE1;
        } else {
            for (thread = __OSActiveThreadQueue.head; thread; thread = thread->linkActive.next) {
                thread->context.srr1 &= ~(MSR_FE0 | MSR_FE1);
                thread->context.fpscr &= ~(FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE);
                thread->context.fpscr &= FPSCR_STATUS_MASK;
            }
            fpscr &= ~(FPSCR_VE | FPSCR_OE | FPSCR_UE | FPSCR_ZE | FPSCR_XE);
            msr &= ~(MSR_FE0 | MSR_FE1);
        }
        fpscr &= FPSCR_STATUS_MASK;
        PPCMtfpscr(fpscr);
        PPCMtmsr(msr);
    }

    OSRestoreInterrupts(enabled);
    return oldHandler;
}

void __OSUnhandledException(unsigned char exception, struct OSContext* context, unsigned long dsisr, unsigned long dar) {
    OSTime time;
    u32 msr;

    time = OSGetTime();

    if (!(context->srr1 & MSR_RI)) {
        OSReport("Non-recoverable Exception %d", exception);
    } else {
        if (exception == OS_ERROR_PROGRAM && (context->srr1 & SRR1_FP_EXCEPTION) &&
            __OSErrorTable[__OS_ERROR_FPU_EXCEPTION]) {
            exception = __OS_ERROR_FPU_EXCEPTION;
            msr = PPCMfmsr();
            PPCMtmsr(msr | MSR_FP);
            if (__OSFPUContext) {
                OSSaveFPUContext(__OSFPUContext);
            }
            PPCMtfpscr(PPCMffpscr() & FPSCR_STATUS_MASK);
            PPCMtmsr(msr);
            if (__OSFPUContext == context) {
                OSDisableScheduler();
                __OSErrorTable[exception](exception, context, dsisr, dar);
                context->srr1 &= ~MSR_FP;
                __OSFPUContext = NULL;
                context->fpscr &= FPSCR_STATUS_MASK;
                OSEnableScheduler();
                __OSReschedule();
            } else {
                context->srr1 &= ~MSR_FP;
                __OSFPUContext = NULL;
            }
            OSLoadContext(context);
        }
        if (__OSErrorTable[exception]) {
            OSDisableScheduler();
            __OSErrorTable[exception](exception, context, dsisr, dar);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }
        if (exception == __OS_EXCEPTION_DECREMENTER) {
            OSLoadContext(context);
        }
        OSReport("Unhandled Exception %d", exception);
    }
    OSReport("\n");
    OSDumpContext(context);
    OSReport("\nDSISR = 0x%08x                   DAR  = 0x%08x\n", dsisr, dar);
    OSReport("TB = 0x%016llx\n", time);

    switch (exception) {
        case __OS_EXCEPTION_DSI:
            OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access "
                     "invalid address 0x%x (read from DAR)\n",
                     context->srr0, dar);
            break;
        case __OS_EXCEPTION_ISI:
            OSReport("\nAttempted to fetch instruction from invalid address 0x%x "
                     "(read from SRR0)\n",
                     context->srr0);
            break;
        case __OS_EXCEPTION_ALIGNMENT:
            OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access "
                     "unaligned address 0x%x (read from DAR)\n",
                     context->srr0, dar);
            break;
        case __OS_EXCEPTION_PROGRAM:
            OSReport("\nProgram exception : Possible illegal instruction/operation "
                     "at or around 0x%x (read from SRR0)\n",
                     context->srr0, dar);
            break;
        case __OS_EXCEPTION_PROTECTION:
            OSReport("\n");
            OSReport("AI DMA Address =   0x%04x%04x\n", __DSPRegs[DSP_DMA_START_HI],
                     __DSPRegs[DSP_DMA_START_LO]);
            OSReport("ARAM DMA Address = 0x%04x%04x\n", __DSPRegs[DSP_ARAM_DMA_MM_HI],
                     __DSPRegs[DSP_ARAM_DMA_MM_LO]);
            OSReport("DI DMA Address =   0x%08x\n", __DIRegs[5]);
            break;
    }

    OSReport("\nLast interrupt (%d): SRR0 = 0x%08x  TB = 0x%016llx\n", __OSLastInterrupt,
             __OSLastInterruptSrr0, __OSLastInterruptTime);
    PPCHalt();
}
