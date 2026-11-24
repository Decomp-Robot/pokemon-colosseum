#include <dolphin.h>
#include <dolphin/hw_regs.h>
#include <dolphin/os.h>

#include "__os.h"

#define MEM_SIZE_24MB (24 * 1024 * 1024)
#define MEM_SIZE_48MB (48 * 1024 * 1024)

#define MI_PROTECT_ALL_RDWR 0xFF
#define MI_FAULT_ADDR_UPPER_MASK 0x3FF
#define MI_FAULT_ADDR_UPPER_SHIFT 16
#define MI_MEM_CONFIG_24MB 2

#define OS_PROTECT_PAGE_SIZE 1024
#define OS_PROTECT_PAGE_SHIFT 10
#define OS_PROTECT_CONTROL_MASK 3

#define OS_CONSOLE_SIM_MEM_SIZE_OFFSET 0xF0

u32 OSGetPhysicalMemSize(void) {
    OSBootInfo * BootInfo = (OSBootInfo *)OSPhysicalToCached(0);

    return BootInfo->memorySize;
}

u32 OSGetConsoleSimulatedMemSize(void) {
    u32 * memSize = (u32 *)OSPhysicalToCached(OS_CONSOLE_SIM_MEM_SIZE_OFFSET);

    return *memSize;
}

static BOOL OnReset(BOOL final) {
    if (final) {
        __MEMRegs[MI_PROTECT] = MI_PROTECT_ALL_RDWR;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                           OS_INTERRUPTMASK_MEM_3);
    }

    return TRUE;
}

static void MEMIntrruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u32 addr;
    u32 cause;

    addr = ((__MEMRegs[MI_FAULT_ADDR_HIGH] & MI_FAULT_ADDR_UPPER_MASK) << MI_FAULT_ADDR_UPPER_SHIFT) |
           __MEMRegs[MI_FAULT_ADDR_LOW];
    cause = __MEMRegs[MI_CAUSE];
    __MEMRegs[MI_INT_CLEAR] = 0;

    if (__OSErrorTable[OS_ERROR_PROTECTION] != NULL) {
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, cause, addr);
    } else {
        __OSUnhandledException(OS_ERROR_PROTECTION, context, cause, addr);
    }
}

void OSProtectRange(u32 chan, void* addr, u32 nBytes, u32 control) {
    BOOL enabled;
    u32 start;
    u32 end;
    u16 reg;

    if (chan < OS_PROTECT_CHAN_MAX) {
        start = (u32)addr & ~(OS_PROTECT_PAGE_SIZE - 1);
        end = (u32)addr + nBytes;
        end = (end + OS_PROTECT_PAGE_SIZE - 1) & ~(OS_PROTECT_PAGE_SIZE - 1);
        control &= OS_PROTECT_CONTROL_MASK;
        DCFlushRange((void*)start, end - start);

        enabled = OSDisableInterrupts();
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 >> chan);

        __MEMRegs[MI_MARR(chan)] = start >> OS_PROTECT_PAGE_SHIFT;
        __MEMRegs[MI_MARR(chan) + 1] = end >> OS_PROTECT_PAGE_SHIFT;

        reg = __MEMRegs[MI_PROTECT];
        reg &= ~(OS_PROTECT_CONTROL_MASK << (chan * 2));
        reg |= control << (chan * 2);
        __MEMRegs[MI_PROTECT] = reg;

        if (control != OS_PROTECT_CONTROL_RDWR) {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_0 >> chan);
        }

        OSRestoreInterrupts(enabled);
    }
}

static asm void Config24MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x0000
    addi r4, r4, 0x0002
    lis r3, 0x8000
    addi r3, r3, 0x01FF
    lis r6, 0x0100
    addi r6, r6, 0x0002
    lis r5, 0x8100
    addi r5, r5, 0x00FF
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, (MSR_IR | MSR_DR)
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void Config48MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x0000
    addi r4, r4, 0x0002
    lis r3, 0x8000
    addi r3, r3, 0x03FF
    lis r6, 0x0200
    addi r6, r6, 0x0002
    lis r5, 0x8200
    addi r5, r5, 0x01FF
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, (MSR_IR | MSR_DR)
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void RealMode(register void (*func)(void)) {
    nofralloc
    clrlwi func, func, 2
    mtsrr0 func
    mfmsr r3
    rlwinm r3, r3, 0, MSR_DR_BIT + 1, MSR_IR_BIT - 1
    mtsrr1 r3
    rfi
}

static OSResetFunctionInfo ResetFunctionInfo = {OnReset, 127, NULL, NULL};

void __OSInitMemoryProtection(void) {
    u32 memSize;
    BOOL enabled;

    memSize = OSGetConsoleSimulatedMemSize();
    enabled = OSDisableInterrupts();
    __MEMRegs[MI_INT_CLEAR] = 0;
    __MEMRegs[MI_PROTECT] = MI_PROTECT_ALL_RDWR;
    __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                       OS_INTERRUPTMASK_MEM_3);

    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_0, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_1, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_2, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_3, MEMIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_MEM_ADDRESS, MEMIntrruptHandler);

    OSRegisterResetFunction(&ResetFunctionInfo);

    if (OSGetConsoleSimulatedMemSize() < OSGetPhysicalMemSize() && OSGetConsoleSimulatedMemSize() == MEM_SIZE_24MB) {
        DCInvalidateRange((void*)(OS_BASE_CACHED + MEM_SIZE_24MB), MEM_SIZE_24MB);
        __MEMRegs[MI_MEM_CONFIG] = MI_MEM_CONFIG_24MB;
    }

    if (memSize <= MEM_SIZE_24MB) {
        RealMode(Config24MB);
    } else if (memSize <= MEM_SIZE_48MB) {
        RealMode(Config48MB);
    }

    __OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_ADDRESS);
    OSRestoreInterrupts(enabled);
}
