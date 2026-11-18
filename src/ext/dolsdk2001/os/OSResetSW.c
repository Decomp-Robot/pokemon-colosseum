#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

long long Hold;
long long LastResetEnd;
int ResetButtonState;
int Down;
OSResetCallback ResetCallback;

volatile u8 __OSBootFlag : 0x800030E3;

void __OSResetSWInterruptHandler(short exception, struct OSContext *context) {
    OSResetCallback callback;

    Hold = __OSGetSystemTime();
    while ((__OSGetSystemTime() - Hold) < OSMicrosecondsToTicks(100) && !(__PIRegs[0] & 0x10000)) {
    }

    if (!(__PIRegs[0] & 0x10000)) {
        Down = 1;
        ResetButtonState = 1;
        __OSMaskInterrupts(0x200);

        if (ResetCallback) {
            callback = ResetCallback;
            ResetCallback = NULL;
            callback();
        }
    }
    __PIRegs[0] = 2;
}

BOOL OSGetResetButtonState(void) {
    BOOL enabled;
    BOOL state;
    OSTime now;
    OSTime start;
    u8 bootFlag;

    enabled = OSDisableInterrupts();
    now = __OSGetSystemTime();

    if (!(__PIRegs[0] & 0x10000)) {
        if (!Down) {
            Down = TRUE;
            state = (LastResetEnd != 0) ? TRUE : FALSE;
            Hold = now;
        } else {
            state = ((LastResetEnd != 0) || (OSMicrosecondsToTicks(100) < (now - Hold))) ? TRUE : FALSE;
        }
    } else if (Down) {
        Down = FALSE;
        state = ResetButtonState;
        if (ResetButtonState != 0) {
            LastResetEnd = now;
        } else {
            LastResetEnd = 0;
        }
    } else if ((LastResetEnd != 0) && ((now - LastResetEnd) < OSMillisecondsToTicks(40))) {
        state = TRUE;
    } else {
        state = FALSE;
        LastResetEnd = 0;
    }
    ResetButtonState = state;

    bootFlag = __OSBootFlag & 0x3F;
    if (bootFlag) {
        start = bootFlag * 60;
        start = __OSStartTime + OSSecondsToTicks(start);
        if (now > start) {
            if (((OSTicksToSeconds(now - start) / 2) & 1) == 0) {
                state = TRUE;
            } else {
                state = FALSE;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return state;
}

// UNUSED: the game never calls this; dead-stripped at link.
OSResetCallback OSSetResetCallback(OSResetCallback callback) {
    int enabled;
    OSResetCallback prevCallback;

    enabled = OSDisableInterrupts();
    prevCallback = ResetCallback;
    ResetCallback = callback;

    if (callback) {
        __PIRegs[0] = 2;
        __OSUnmaskInterrupts(0x200);
    } else {
        __OSMaskInterrupts(0x200);
    }
    OSRestoreInterrupts(enabled);
    return prevCallback;
}

// UNUSED: the game never calls this; dead-stripped at link.
