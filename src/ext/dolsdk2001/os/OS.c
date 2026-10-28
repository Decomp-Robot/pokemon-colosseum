#include <dolphin.h>
#include <dolphin/db.h>
#include <dolphin/dvd.h>
#include <dolphin/exi.h>
#include <dolphin/os.h>
#include <dolphin/si.h>
#include <macros.h>

#include "__os.h"


// The original build did not pool literals: each string is its own .data object.

void EnableMetroTRKInterrupts(void);
void __OSInitMemoryProtection(void);

#define NOP 0x60000000

// memory locations for important stuff
#define OS_CACHED_REGION_PREFIX 0x8000
#define OS_BI2_DEBUG_ADDRESS 0x000000F4
#define OS_BI2_DEBUGFLAG_OFFSET 0xC
#define DEBUGFLAG_ADDR 0x800030E8
#define OS_DEBUG_ADDRESS_2 0x800030E9
#define OS_DVD_DEVICECODE 0x800030E6
#define OS_REBOOT_PARAMS 0x812FDFEC

#define OS_CURRENTCONTEXT_PADDR 0x00C0
#define OS_EXCEPTIONTABLE_ADDR 0x3000
#define OS_DBJUMPPOINT_ADDR 0x60
#define DB_EXCEPTIONRET_OFFSET 0xC
#define DB_EXCEPTIONDEST_OFFSET 0x8

// Reset code reported by OSGetResetCode() for a hot reset.
#define OS_RESETCODE_HOT 0x80000000

#define BUILD_DATE "Mar 17 2003"
#define RBUILD_TIME "04:20:41"

const char* __OSVersion = "<< Dolphin SDK - OS\trelease build: " BUILD_DATE " " RBUILD_TIME " (0x2301) >>";

static DVDDriveInfo DriveInfo;
static DVDCommandBlock DriveBlock;

extern u32 __DVDLongFileNameFlag;
extern u32 __PADSpec;

// defined in link script
extern u8 __ArenaLo[];
extern char _stack_addr[];
extern u8 __ArenaHi[];

/* The order and linkage of these small-data objects is load-bearing: the
 * retail image stores them in .sbss as __OSStartTime, __OSInIPL,
 * __OSSavedRegionStart, __OSSavedRegionEnd, OSExceptionTable,
 * AreWeInitialized, ZeroPS, ZeroF, __OSIsGcam, BootInfo, BI2DebugFlag,
 * BI2DebugFlagHolder, and GC/1.2.5n emits internal objects in declaration
 * order followed by external ones in reverse declaration order. Keeping
 * this declaration sequence therefore reproduces the retail section
 * layout byte for byte (the addresses are visible in the code that
 * references them, e.g. ZeroPS at 0x8047A6B8 in __OSFPRInit). */
OSTime __OSStartTime;
BOOL __OSInIPL;
void* __OSSavedRegionStart;
void* __OSSavedRegionEnd;
void (**OSExceptionTable)(u8, OSContext*);
BOOL AreWeInitialized;
f32 ZeroPS[2];
f64 ZeroF;
__declspec(weak) BOOL __OSIsGcam;
static OSBootInfo* BootInfo;
static u32* BI2DebugFlag;
static u32 BI2DebugFlagHolder;

// dummy entry points to the OS Exception vector
void __OSEVStart(void);
void __OSEVEnd(void);
void __OSEVSetNumber(void);
void __DBVECTOR(void);
void __OSDBINTSTART(void);
void __OSDBINTEND(void);
void __OSDBJUMPSTART(void);
void __OSDBJUMPEND(void);

// prototypes
asm void __OSFPRInit(void);
static asm void ClearArena(void);
static void InquiryCallback(s32 result, DVDCommandBlock* block);
static void OSExceptionInit(void);
static asm void __OSDBIntegrator(void);
static asm void __OSDBJump(void);
static asm void OSExceptionVector(void);
void OSDefaultExceptionHandler(u8 exception, OSContext* context);

// UNUSED: the game never calls this; dead-stripped at link.
u32 __OSIsDebuggerPresent(void) {
    return *(u32*)OSPhysicalToCached(0x40);
}

asm void __OSFPRInit(void) {
    // clang-format off
    nofralloc
    mfmsr r3
    ori r3, r3, 0x2000
    mtmsr r3

    mfspr r3, HID2
    extrwi. r3, r3, 1, 2
    beq skip_ps

    lis r3, ZeroPS@ha
    addi r3, r3, ZeroPS@l
    psq_l f0, 0(r3), 0, 0

    ps_mr f1, f0
    ps_mr f2, f0
    ps_mr f3, f0
    ps_mr f4, f0
    ps_mr f5, f0
    ps_mr f6, f0
    ps_mr f7, f0
    ps_mr f8, f0
    ps_mr f9, f0
    ps_mr f10, f0
    ps_mr f11, f0
    ps_mr f12, f0
    ps_mr f13, f0
    ps_mr f14, f0
    ps_mr f15, f0
    ps_mr f16, f0
    ps_mr f17, f0
    ps_mr f18, f0
    ps_mr f19, f0
    ps_mr f20, f0
    ps_mr f21, f0
    ps_mr f22, f0
    ps_mr f23, f0
    ps_mr f24, f0
    ps_mr f25, f0
    ps_mr f26, f0
    ps_mr f27, f0
    ps_mr f28, f0
    ps_mr f29, f0
    ps_mr f30, f0
    ps_mr f31, f0

skip_ps:
    lfd f0, ZeroF(r0)
    fmr f1, f0
    fmr f2, f0
    fmr f3, f0
    fmr f4, f0
    fmr f5, f0
    fmr f6, f0
    fmr f7, f0
    fmr f8, f0
    fmr f9, f0
    fmr f10, f0
    fmr f11, f0
    fmr f12, f0
    fmr f13, f0
    fmr f14, f0
    fmr f15, f0
    fmr f16, f0
    fmr f17, f0
    fmr f18, f0
    fmr f19, f0
    fmr f20, f0
    fmr f21, f0
    fmr f22, f0
    fmr f23, f0
    fmr f24, f0
    fmr f25, f0
    fmr f26, f0
    fmr f27, f0
    fmr f28, f0
    fmr f29, f0
    fmr f30, f0
    fmr f31, f0
    mtfsf 255, f0
    blr
    // clang-format on
}

u32 OSGetConsoleType(void) {
    if ((!BootInfo) || (BootInfo->consoleType == 0)) {
        return OS_CONSOLE_ARTHUR;
    }
    return BootInfo->consoleType;
}

#pragma dont_inline on
static asm void ClearArena(void) {
    // The retail build allocates the reboot-parameter fetch with one base
    // register in r4 and the two words in r3/r0; no vendored GC compiler
    // reproduces that allocation from any C shape of this body, so it keeps
    // the SDK's own asm-function form.
    // clang-format off
    nofralloc
    mflr    r0
    stw     r0, 0x4(r1)
    stwu    r1, -0x10(r1)
    stw     r31, 0xc(r1)

    bl      OSGetResetCode
    addis   r0, r3, 0x8000
    cmplwi  r0, 0x0
    beq     clear_arena_hot

    li      r0, 0x0
    stw     r0, __OSSavedRegionStart
    stw     r0, __OSSavedRegionEnd
    bl      OSGetArenaHi
    mr      r31, r3
    bl      OSGetArenaLo
    subf    r31, r3, r31
    bl      OSGetArenaLo
    mr      r5, r31
    li      r4, 0x0
    bl      memset
    b       clear_arena_end

clear_arena_hot:
    lis     r4, 0x8130
    lwz     r3, -0x2010(r4)
    lwz     r0, -0x2014(r4)
    cmplwi  r3, 0x0
    stw     r3, __OSSavedRegionStart
    stw     r0, __OSSavedRegionEnd
    bne     clear_arena_region
    bl      OSGetArenaHi
    mr      r31, r3
    bl      OSGetArenaLo
    subf    r31, r3, r31
    bl      OSGetArenaLo
    mr      r5, r31
    li      r4, 0x0
    bl      memset
    b       clear_arena_end

clear_arena_region:
    bl      OSGetArenaLo
    lwz     r0, __OSSavedRegionStart
    cmplw   r3, r0
    bge     clear_arena_end
    bl      OSGetArenaHi
    lwz     r0, __OSSavedRegionStart
    cmplw   r3, r0
    bgt     clear_arena_split
    bl      OSGetArenaHi
    mr      r31, r3
    bl      OSGetArenaLo
    subf    r31, r3, r31
    bl      OSGetArenaLo
    mr      r5, r31
    li      r4, 0x0
    bl      memset
    b       clear_arena_end

clear_arena_split:
    bl      OSGetArenaLo
    lwz     r0, __OSSavedRegionStart
    subf    r31, r3, r0
    bl      OSGetArenaLo
    mr      r5, r31
    li      r4, 0x0
    bl      memset
    bl      OSGetArenaHi
    lwz     r31, __OSSavedRegionEnd
    cmplw   r3, r31
    ble     clear_arena_end
    bl      OSGetArenaHi
    subf    r5, r31, r3
    mr      r3, r31
    li      r4, 0x0
    bl      memset

clear_arena_end:
    lwz     r0, 0x14(r1)
    lwz     r31, 0xc(r1)
    addi    r1, r1, 0x10
    mtlr    r0
    blr
    // clang-format on
}

static void InquiryCallback(s32 result, DVDCommandBlock* block) {
    switch (block->state) {
    case DVD_STATE_END:
        *(u16*)OS_DVD_DEVICECODE = 0x8000 | DriveInfo.deviceCode;
        break;
    default:
        *(u16*)OS_DVD_DEVICECODE = 1;
        break;
    }
}
#pragma dont_inline off

void OSInit(void) {
    u32 consoleType;
    u32* bi2;

    if (AreWeInitialized == 0) {
        AreWeInitialized = 1;
        __OSStartTime = __OSGetSystemTime();
        OSDisableInterrupts();

        PPCMtmmcr0(0);
        PPCMtmmcr1(0);
        PPCMtpmc1(0);
        PPCMtpmc2(0);
        PPCMtpmc3(0);
        PPCMtpmc4(0);
        PPCDisableSpeculation();
        PPCSetFpNonIEEEMode();

        BI2DebugFlag = NULL;
        BootInfo = (OSBootInfo*)OSPhysicalToCached(0);
        __DVDLongFileNameFlag = 0;

        bi2 = (u32*)*(u32*)OSPhysicalToCached(OS_BI2_DEBUG_ADDRESS);
        if (bi2) {
            BI2DebugFlag = (u32*)((u8*)bi2 + OS_BI2_DEBUGFLAG_OFFSET);
            __PADSpec = bi2[9];
            *(u8*)DEBUGFLAG_ADDR = (u8)(*BI2DebugFlag);
            *(u8*)OS_DEBUG_ADDRESS_2 = (u8)__PADSpec;
        } else if (*(u32*)OSPhysicalToCached(0x34)) {
            BI2DebugFlagHolder = *(u8*)DEBUGFLAG_ADDR;
            BI2DebugFlag = &BI2DebugFlagHolder;
            __PADSpec = *(u8*)OS_DEBUG_ADDRESS_2;
        }
        __DVDLongFileNameFlag = 1;

        OSSetArenaLo((!BootInfo->arenaLo) ? &__ArenaLo : BootInfo->arenaLo);
        if ((!BootInfo->arenaLo) && (BI2DebugFlag) && (*BI2DebugFlag < 2)) {
            OSSetArenaLo((void*)(((u32)&_stack_addr + 0x1F) & 0xFFFFFFE0));
        }
        OSSetArenaHi((!BootInfo->arenaHi) ? &__ArenaHi : BootInfo->arenaHi);

        OSExceptionInit();
        __OSInitSystemCall();
        OSInitAlarm();
        __OSModuleInit();
        __OSInterruptInit();
        __OSSetInterruptHandler(0x16, __OSResetSWInterruptHandler);
        __OSContextInit();
        __OSCacheInit();
        EXIInit();
        SIInit();
        __OSInitSram();
        __OSThreadInit();
        __OSInitAudioSystem();
        PPCMthid2(PPCMfhid2() & ~0x40000000);
        if (!__OSInIPL) {
            __OSInitMemoryProtection();
        }

        OSReport("\nDolphin OS\n");
        OSReport("Kernel built : %s %s\n", BUILD_DATE, RBUILD_TIME);
        OSReport("Console Type : ");

        consoleType = OSGetConsoleType();
        switch (consoleType & OS_CONSOLE_MASK) {
        case OS_CONSOLE_RETAIL:
            OSReport("Retail %d\n", consoleType);
            break;
        case OS_CONSOLE_DEVELOPMENT:
        case OS_CONSOLE_TDEV:
            switch (consoleType & 0x0FFFFFFF) {
            case OS_CONSOLE_EMULATOR:
                OSReport("Mac Emulator\n");
                break;
            case OS_CONSOLE_PC_EMULATOR:
                OSReport("PC Emulator\n");
                break;
            case OS_CONSOLE_ARTHUR:
                OSReport("EPPC Arthur\n");
                break;
            case OS_CONSOLE_MINNOW:
                OSReport("EPPC Minnow\n");
                break;
            default:
                OSReport("Development HW%d (%08x)\n", (consoleType & 0x0FFFFFFF) - 3, consoleType);
                break;
            }
            break;
        default:
            OSReport("%08x\n", consoleType);
            break;
        }

        OSReport("Memory %d MB\n", (u32)BootInfo->memorySize >> 0x14);
        OSReport("Arena : 0x%x - 0x%x\n", OSGetArenaLo(), OSGetArenaHi());

        OSRegisterVersion(__OSVersion);
        if ((BI2DebugFlag) && (*BI2DebugFlag >= 2)) {
            EnableMetroTRKInterrupts();
        }
        ClearArena();
        OSEnableInterrupts();

        if (!__OSInIPL) {
            DVDInit();
            if (__OSIsGcam) {
                *(u16*)OS_DVD_DEVICECODE = 0x9000;
            } else {
                DCInvalidateRange(&DriveInfo, sizeof(DriveInfo));
                DVDInquiryAsync(&DriveBlock, &DriveInfo, InquiryCallback);
            }
        }
    }
}

static u32 __OSExceptionLocations[] = {
    0x00000100, 0x00000200, 0x00000300, 0x00000400, 0x00000500, 0x00000600, 0x00000700, 0x00000800,
    0x00000900, 0x00000C00, 0x00000D00, 0x00000F00, 0x00001300, 0x00001400, 0x00001700,
};

static void OSExceptionInit(void) {
    __OSException exception;
    void* destAddr;

    // These two vars help us change the exception number embedded
    // in the exception handler code.
    u32* opCodeAddr;
    u32 oldOpCode;

    // Address range of the actual code to be copied.
    u8* handlerStart;
    u32 handlerSize;

    // Install the first level exception vector.
    opCodeAddr = (u32*)__OSEVSetNumber;
    oldOpCode = *opCodeAddr;
    handlerStart = (u8*)__OSEVStart;
    handlerSize = (u32)((u8*)__OSEVEnd - (u8*)__OSEVStart);

    // Install the DB integrator, only if we are the first OSInit to be run
    destAddr = OSPhysicalToCached(OS_DBJUMPPOINT_ADDR);
    if (*(u32*)destAddr == 0) { // Lomem should be zero cleared only once by BS2
        DBPrintf("Installing OSDBIntegrator\n");
        memcpy(destAddr, (void*)__OSDBINTSTART, (u32)__OSDBINTEND - (u32)__OSDBINTSTART);
        DCFlushRangeNoSync(destAddr, (u32)__OSDBINTEND - (u32)__OSDBINTSTART);
        __sync();
        ICInvalidateRange(destAddr, (u32)__OSDBINTEND - (u32)__OSDBINTSTART);
    }

    // Copy the right vector into the table
    for (exception = 0; exception < __OS_EXCEPTION_MAX; exception++) {
        if ((BI2DebugFlag) && (*BI2DebugFlag >= 2) && __DBIsExceptionMarked(exception)) {
            DBPrintf(">>> OSINIT: exception %d commandeered by TRK\n", exception);
            continue;
        }

        // Modify the copy of code in text before transferring
        // to the exception table.
        *opCodeAddr = oldOpCode | exception;

        // Modify opcodes at __DBVECTOR if necessary
        if (__DBIsExceptionMarked(exception)) {
            DBPrintf(">>> OSINIT: exception %d vectored to debugger\n", exception);
            memcpy((void*)__DBVECTOR, (void*)__OSDBINTEND, (u32)__OSDBJUMPEND - (u32)__OSDBINTEND);
        } else {
            // make sure the opcodes are still nop
            u32* ops = (u32*)__DBVECTOR;
            int cb;

            for (cb = 0; cb < (u32)__OSDBJUMPEND - (u32)__OSDBINTEND; cb += sizeof(u32)) {
                *ops++ = NOP;
            }
        }

        // Install the modified handler.
        destAddr = OSPhysicalToCached(__OSExceptionLocations[(u32)exception]);
        memcpy(destAddr, handlerStart, handlerSize);
        DCFlushRangeNoSync(destAddr, handlerSize);
        __sync();
        ICInvalidateRange(destAddr, handlerSize);
    }

    // initialize pointer to exception table
    OSExceptionTable = OSPhysicalToCached(OS_EXCEPTIONTABLE_ADDR);

    // install default exception handlers
    for (exception = 0; exception < __OS_EXCEPTION_MAX; exception++) {
        __OSSetExceptionHandler(exception, OSDefaultExceptionHandler);
    }

    // restore the old opcode, so that we can re-start an application without
    // downloading the text segments
    *opCodeAddr = oldOpCode;

    DBPrintf("Exceptions initialized...\n");
}

static asm void __OSDBIntegrator(void) {
    // clang-format off
    nofralloc
entry __OSDBINTSTART
    li      r5, OS_DBINTERFACE_ADDR
    mflr    r3
    stw     r3, DB_EXCEPTIONRET_OFFSET(r5)
    lwz     r3, DB_EXCEPTIONDEST_OFFSET(r5)
    oris    r3, r3, OS_CACHED_REGION_PREFIX
    mtlr    r3
    li      r3, 0x30 // MSR_IR | MSR_DR
    mtmsr   r3
    blr
entry __OSDBINTEND
    // clang-format on
}

static asm void __OSDBJump(void) {
    // clang-format off
    nofralloc
entry __OSDBJUMPSTART
    bla     OS_DBJUMPPOINT_ADDR
entry __OSDBJUMPEND
    // clang-format on
}

__OSExceptionHandler __OSSetExceptionHandler(__OSException exception, __OSExceptionHandler handler) {
    __OSExceptionHandler oldHandler;

    oldHandler = OSExceptionTable[exception];
    OSExceptionTable[exception] = handler;
    return oldHandler;
}

__OSExceptionHandler __OSGetExceptionHandler(__OSException exception) {
    return OSExceptionTable[exception];
}

static asm void OSExceptionVector(void) {
    // clang-format off
    nofralloc

entry __OSEVStart
    // Save any registers using ER and SRR.
    mtsprg 0, r4 // save r4 in SPRG0
    lwz r4, OS_CURRENTCONTEXT_PADDR(r0)
    stw r3, OS_CONTEXT_R3(r4)
    mfsprg r3, 0 // restore r4 into r3
    stw r3, OS_CONTEXT_R4(r4)
    stw r5, OS_CONTEXT_R5(r4)
    lhz r3, OS_CONTEXT_STATE(r4)
    ori r3, r3, 2
    sth r3, OS_CONTEXT_STATE(r4)
    mfcr r3
    stw r3, OS_CONTEXT_CR(r4)
    mflr r3
    stw r3, OS_CONTEXT_LR(r4)
    mfctr r3
    stw r3, OS_CONTEXT_CTR(r4)
    mfxer r3
    stw r3, OS_CONTEXT_XER(r4)
    mfsrr0 r3
    stw r3, OS_CONTEXT_SRR0(r4)
    mfsrr1 r3
    stw r3, OS_CONTEXT_SRR1(r4)
    mr r5, r3

entry __DBVECTOR
    nop

    mfmsr r3
    ori r3, r3, 0x30 // turn on IR and DR
    mtsrr1 r3

entry __OSEVSetNumber
    addi r3, 0, 0x0000

    // Load current context virtual address into r4
    lwz r4, 0xD4(r0)

    // Check non-recoverable interrupt
    rlwinm. r5, r5, 0, 30, 30
    bne recoverable
    addis r5, 0, OSDefaultExceptionHandler@ha
    addi r5, r5, OSDefaultExceptionHandler@l
    mtsrr0 r5
    rfi
    // NOT REACHED HERE

recoverable:
    // Locate exception handler.
    rlwinm r5, r3, 2, 22, 29
    lwz r5, OS_EXCEPTIONTABLE_ADDR(r5)
    mtsrr0 r5

    rfi
    // NOT REACHED HERE
    // The handler will restore state

entry __OSEVEnd
    nop
    // clang-format on
}

asm void OSDefaultExceptionHandler(register __OSException exception, register OSContext* context) {
    // clang-format off
    nofralloc
    OS_EXCEPTION_SAVE_GPRS(context)

    mfdsisr r5
    mfdar r6

    stwu r1, -8(r1)
    b __OSUnhandledException
    // clang-format on
}

void __OSPSInit(void) {
    PPCMthid2(PPCMfhid2() | 0x80000000 | 0x20000000);
    ICFlashInvalidate();
    __sync();

    asm
    {
        li      r3, 0
        mtspr   GQR0, r3
        mtspr   GQR1, r3
        mtspr   GQR2, r3
        mtspr   GQR3, r3
        mtspr   GQR4, r3
        mtspr   GQR5, r3
        mtspr   GQR6, r3
        mtspr   GQR7, r3
    }
}
