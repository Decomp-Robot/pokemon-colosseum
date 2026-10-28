#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

// These macros are copied from OSThread.c. Or ARE they the same
// macros? They dont seem to be in the SDK headers.
#define ENQUEUE_INFO(info, queue)                            \
    do {                                                     \
        struct OSResetFunctionInfo * __prev = (queue)->tail; \
        if (__prev == 0) {                                   \
            (queue)->head = (info);                          \
        } else {                                             \
            __prev->next = (info);                           \
        }                                                    \
        (info)->prev = __prev;                               \
        (info)->next = 0;                                    \
        (queue)->tail = (info);                              \
    } while(0);

#define DEQUEUE_INFO(info, queue)                           \
    do {                                                    \
        struct OSResetFunctionInfo * __next = (info)->next; \
        struct OSResetFunctionInfo * __prev = (info)->prev; \
        if (__next == 0) {                                  \
            (queue)->tail = __prev;                         \
        } else {                                            \
            __next->prev = __prev;                          \
        }                                                   \
        if (__prev == 0) {                                  \
            (queue)->head = __next;                         \
        } else {                                            \
            __prev->next = __next;                          \
        }                                                   \
    } while(0);

#define ENQUEUE_INFO_PRIO(info, queue)               \
    do {                                             \
        struct OSResetFunctionInfo * __prev;         \
        struct OSResetFunctionInfo * __next;         \
        for(__next = (queue)->head; __next           \
          && (__next->priority <= (info)->priority); \
                __next = __next->next) ;             \
                                                     \
        if (__next == 0) {                           \
            ENQUEUE_INFO(info, queue);               \
        } else {                                     \
            (info)->next = __next;                   \
            __prev = __next->prev;                   \
            __next->prev = (info);                   \
            (info)->prev = __prev;                   \
            if (__prev == 0) {                       \
                (queue)->head = (info);              \
            } else {                                 \
                __prev->next = (info);               \
            }                                        \
        }                                            \
    } while(0);

// The functions below are used by this file but are not prototyped by the
// headers shipped with this revision of the library.
BOOL __PADDisableRecalibration(BOOL disable);
void __OSReboot(unsigned long resetCode, int forceMenu);
void OSCancelThread(struct OSThread * thread);

static struct OSResetFunctionQueue ResetFunctionQueue;

// The game's boot code at 0x8009CD38 (zz_8009CD38_) walks the reset function
// list through this entry point, calling it at 0x8009CDEC (final = 0) and at
// 0x8009CE28 (final = 1), so it is an exported entry point of the library and
// not a file-local helper.
int CallResetFunctions(int final);

void OSRegisterResetFunction(struct OSResetFunctionInfo * info) {
    ASSERTLINE(0x76, info->func);

    ENQUEUE_INFO_PRIO(info, &ResetFunctionQueue);
}

// UNUSED: the game never calls this; dead-stripped at link.
void OSUnregisterResetFunction(struct OSResetFunctionInfo * info) {
    DEQUEUE_INFO(info, &ResetFunctionQueue);
}

int CallResetFunctions(int final) {
    struct OSResetFunctionInfo * info;
    int err = 0;

    for(info = ResetFunctionQueue.head; info && !err; info = info->next) {
        err |= !info->func(final);
    }
    err |= !__OSSyncSram();
    if (err) {
        return 0;
    }
    return 1;
}

static asm void Reset(unsigned long resetCode) {
    nofralloc
    b L_000001BC
L_000001A0:
    mfspr r8, HID0
    ori r8, r8, 0x8
    mtspr HID0, r8
    isync
    sync
    nop
    b L_000001C0
L_000001BC:
    b L_000001DC
L_000001C0:
    mftb r5, 268
L_000001C4:
    mftb r6, 268
    subf r7, r5, r6
    cmplwi r7, 0x1124
    blt L_000001C4
    nop
    b L_000001E0
L_000001DC:
    b L_000001FC
L_000001E0:
    lis r8, 0xcc00
    ori r8, r8, 0x3000
    li r4, 0x3
    stw r4, 0x24(r8)
    stw r3, 0x24(r8)
    nop
    b L_00000200
L_000001FC:
    b L_00000208
L_00000200:
    nop
    b L_00000200
L_00000208:
    b L_000001A0
}

void __OSDoHotReset(unsigned long resetCode) {
    OSDisableInterrupts();
    __VIRegs[1] = 0;
    ICFlashInvalidate();
    Reset(resetCode * 8);
}

void OSResetSystem(int reset, unsigned long resetCode, int forceMenu) {
    int rc;
    int enabled;
    int rc2;
    BOOL disableRecalibration;
    struct OSSram * sram;
    struct OSThread * thread;
    struct OSThread * next;

    rc2 = OSDisableScheduler();
    __OSStopAudioSystem();
    if (reset == OS_RESET_SHUTDOWN) {
        disableRecalibration = __PADDisableRecalibration(TRUE);
    }
    do {} while (CallResetFunctions(FALSE) == 0);

    if ((reset == OS_RESET_HOTRESET) && (forceMenu != 0)) {
        sram = __OSLockSram();
        sram->flags |= 0x40;
        __OSUnlockSram(1);
        do {} while(__OSSyncSram() == 0);
    }

    enabled = OSDisableInterrupts();
    rc = CallResetFunctions(TRUE);
    ASSERTLINE(0x117, rc);
    LCDisable();
    if (reset == OS_RESET_HOTRESET) {
        __OSDoHotReset(resetCode);
    } else if (reset == OS_RESET_RESTART) {
        for(thread = __OSActiveThreadQueue.head; thread; thread = next) {
            next = thread->linkActive.next;
            switch(thread->state) {
            case 1:
            case 4:
                OSCancelThread(thread);
                continue;
            default:
                continue;
            }
        }
        OSEnableScheduler();
        __OSReboot(resetCode, forceMenu);
    }

    for(thread = __OSActiveThreadQueue.head; thread; thread = next) {
        next = thread->linkActive.next;
        switch(thread->state) {
        case 1:
        case 4:
            OSCancelThread(thread);
            continue;
        default:
            continue;
        }
    }
    memset((void *)(OS_BASE_CACHED | 0x0040), 0, 0x00CC - 0x0040);
    memset((void *)(OS_BASE_CACHED | 0x00D4), 0, 0x00E8 - 0x00D4);
    memset((void *)(OS_BASE_CACHED | 0x00F4), 0, 0x00F8 - 0x00F4);
    memset((void *)(OS_BASE_CACHED | 0x3000), 0, 0x00C0);
    memset((void *)(OS_BASE_CACHED | 0x30C8), 0, 0x00D4 - 0x00C8);
    memset((void *)(OS_BASE_CACHED | 0x30E2), 0, 1);
    __PADDisableRecalibration(disableRecalibration);
}

unsigned long OSGetResetCode() {
    unsigned long resetCode;

    if (*(volatile unsigned char *)(OS_BASE_CACHED | 0x30E2)) {
        resetCode = 0x80000000;
    } else {
        resetCode = (__PIRegs[9] & 0xFFFFFFF8) / 8;
    }
    return resetCode;
}
