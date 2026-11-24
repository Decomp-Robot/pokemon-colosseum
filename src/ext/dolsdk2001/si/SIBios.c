#include <dolphin/os.h>
#include <dolphin/si.h>
#include <dolphin/vi.h>
#include <dolphin/hw_regs.h>

#include "__os.h"

#define SI_BUF_IDX 32
#define SI_POLL_IDX 12

#define SI_CHAN_OUTBUF 0
#define SI_CHAN_INBUFH 1
#define SI_CHAN_INBUFL 2
#define SI_CHAN_STRIDE 3

#define SI_POLL_X_SHIFT 16
#define SI_POLL_Y_SHIFT 8
#define SI_POLL_X_MASK 0x03FF0000
#define SI_POLL_Y_MASK 0x0000FF00
#define SI_POLL_XY_MASK (SI_POLL_X_MASK | SI_POLL_Y_MASK)
#define SI_POLL_EN_MASK 0x000000F0
#define SI_POLL_EN0 0x00000080

#define SI_STATUS_ERROR_MASK 0x0F000000
#define SI_ERROR_MASK \
    (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_COLLISION | SI_ERROR_NO_RESPONSE)

#define SI_TYPE_CMD_LENGTH 1
#define SI_TYPE_REPLY_LENGTH 3
#define SI_WIRELESS_CMD 0x4E000000
#define SI_WIRELESS_CMD_LENGTH 3
#define SI_MAX_TYPE_CALLBACKS 4
#define SI_MAX_TRANSFER_BYTES 128

typedef struct SICsr {
    u32 tcint : 1;
    u32 tcintmsk : 1;
    u32 comerr : 1;
    u32 rdstint : 1;
    u32 rdstintmsk : 1;
    u32 : 4;
    u32 outlngth : 7;
    u32 : 1;
    u32 inlngth : 7;
    u32 : 5;
    u32 chan : 2;
    u32 tstart : 1;
} SICsr;

typedef union {
    u32 val;
    SICsr f;
} SICsr_u;

char* __SIVersion = "<< Dolphin SDK - SI\trelease build: Sep  5 2002 05:33:08 (0x2301) >>";

static struct SIPacket Packet[SI_MAX_CHAN];
static OSAlarm Alarm[SI_MAX_CHAN];
static OSTime TypeTime[SI_MAX_CHAN];
static OSTime TransferTime[SI_MAX_CHAN];
static SITypeAndStatusCallback TypeCallback[SI_MAX_CHAN][SI_MAX_TYPE_CALLBACKS];
static __OSInterruptHandler PollingHandler[SI_MAX_CHAN];
static u32 PollingPending[SI_MAX_CHAN];
static u32 PollingResponse[SI_MAX_CHAN][2];
static volatile u32 PollingLine[SI_MAX_CHAN];
static u32 PollingCommand[SI_MAX_CHAN];

static struct SIControl Si = {CHAN_NONE};
static u32 Type[SI_MAX_CHAN] = {SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE,
                                SI_ERROR_NO_RESPONSE};

u32 __PADFixBits;

static u32 CompleteTransfer(void);
static void SIInterruptHandler(__OSInterrupt interrupt, OSContext* context);
static void GetTypeCallback(s32 chan, u32 error, OSContext* context);
static BOOL SITransferStart(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes,
                            SICallback callback);
static BOOL SIGetResponseRaw(s32 chan);

BOOL SIBusy(void) {
    if (Si.chan != CHAN_NONE) {
        return TRUE;
    }
    return FALSE;
}

BOOL SIIsChanBusy(s32 chan) {
    return Packet[chan].chan != CHAN_NONE || Si.chan == chan;
}

static u32 CompleteTransfer(void) {
    u32 sr;
    u32 i;
    u32 rLen;
    u32* input;
    u8* tail;
    u32 temp;

    sr = __SIRegs[SI_STATUS_IDX];
    __SIRegs[SI_COMCSR_IDX] =
        (__SIRegs[SI_COMCSR_IDX] | SI_COMCSR_TCINT_MASK) & ~SI_COMCSR_TSTART_MASK;

    if (Si.chan != CHAN_NONE) {
        TransferTime[Si.chan] = __OSGetSystemTime();

        input = Si.input;
        rLen = Si.inputBytes / 4;
        for (i = 0; i < rLen; i++) {
            *input++ = __SIRegs[SI_BUF_IDX + i];
        }

        rLen = Si.inputBytes & 3;
        if (rLen) {
            temp = __SIRegs[SI_BUF_IDX + i];
            tail = (u8*)input;
            for (i = 0; i < rLen; i++) {
                *tail++ = (u8)(temp >> ((3 - i) * 8));
            }
        }

        if ((__SIRegs[SI_COMCSR_IDX] & SI_COMCSR_COMERR_MASK) != 0) {
            sr >>= (3 - Si.chan) * 8;
            sr &= SI_ERROR_MASK;
            if ((sr & SI_ERROR_NO_RESPONSE) != 0 && (Type[Si.chan] & SI_ERROR_BUSY) == 0) {
                Type[Si.chan] = SI_ERROR_NO_RESPONSE;
            }
            if (sr == 0) {
                sr = SI_ERROR_COLLISION;
            }
        } else {
            TypeTime[Si.chan] = __OSGetSystemTime();
            sr = 0;
        }
        Si.chan = CHAN_NONE;
    }
    return sr;
}

static inline void SITransferNext(s32 chan) {
    int i;
    struct SIPacket* packet;

    for (i = 0; i < SI_MAX_CHAN; i++) {
        chan++;
        chan %= SI_MAX_CHAN;
        packet = &Packet[chan];
        if (packet->chan != CHAN_NONE && packet->time <= __OSGetSystemTime()) {
            if (SITransferStart(packet->chan, packet->output, packet->outputBytes, packet->input,
                                packet->inputBytes, packet->callback)) {
                OSCancelAlarm(&Alarm[chan]);
                packet->chan = CHAN_NONE;
            }
            return;
        }
    }
}

static void SIInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u32 reg;

    reg = __SIRegs[SI_COMCSR_IDX];
    if ((reg & (SI_COMCSR_TCINT_MASK | SI_COMCSR_TCINTMSK_MASK)) ==
        (SI_COMCSR_TCINT_MASK | SI_COMCSR_TCINTMSK_MASK)) {
        s32 chan;
        u32 sr;
        SICallback callback;

        chan = Si.chan;
        sr = CompleteTransfer();
        callback = Si.callback;
        Si.callback = NULL;

        SITransferNext(chan);

        if (callback) {
            callback(chan, sr, context);
        }

        sr = __SIRegs[SI_STATUS_IDX];
        sr &= SI_STATUS_ERROR_MASK >> (8 * chan);
        __SIRegs[SI_STATUS_IDX] = sr;

        if (Type[chan] == SI_ERROR_BUSY && !SIIsChanBusy(chan)) {
            static u32 cmdTypeAndStatus;

            SITransfer(chan, &cmdTypeAndStatus, SI_TYPE_CMD_LENGTH, &Type[chan],
                       SI_TYPE_REPLY_LENGTH, GetTypeCallback, OSMicrosecondsToTicks(65));
        }
    }

    if ((reg & (SI_COMCSR_RDSTINT_MASK | SI_COMCSR_RDSTINTMSK_MASK)) ==
        (SI_COMCSR_RDSTINT_MASK | SI_COMCSR_RDSTINTMSK_MASK)) {
        int i;
        u32 vcount;
        u32 x;

        vcount = VIGetCurrentLine() + 1;
        x = (Si.poll & SI_POLL_X_MASK) >> SI_POLL_X_SHIFT;

        for (i = 0; i < SI_MAX_CHAN; i++) {
            if (SIGetResponseRaw(i)) {
                PollingLine[i] = vcount;
            }
        }

        for (i = 0; i < SI_MAX_CHAN; i++) {
            if (!(Si.poll & (PAD_CHAN0_BIT >> (24 + i)))) {
                continue;
            }
            if (PollingLine[i] == 0 || (x / 2) + PollingLine[i] < vcount) {
                return;
            }
        }

        for (i = 0; i < SI_MAX_CHAN; i++) {
            PollingLine[i] = 0;
        }

        for (i = 0; i < SI_MAX_CHAN; i++) {
            if (PollingHandler[i]) {
                PollingHandler[i](interrupt, context);
            }
        }
    }
}

static BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    BOOL old;
    u32 csr;
    s32 i;

    enabled = OSDisableInterrupts();
    csr = __SIRegs[SI_COMCSR_IDX];
    old = (csr & SI_COMCSR_RDSTINTMSK_MASK) ? TRUE : FALSE;
    if (enable) {
        for (i = 0; i < SI_MAX_CHAN; i++) {
            PollingLine[i] = 0;
        }
        csr |= SI_COMCSR_RDSTINTMSK_MASK;
    } else {
        csr &= ~SI_COMCSR_RDSTINTMSK_MASK;
    }
    csr &= ~(SI_COMCSR_TCINT_MASK | SI_COMCSR_TSTART_MASK);
    __SIRegs[SI_COMCSR_IDX] = csr;
    OSRestoreInterrupts(enabled);
    return old;
}

BOOL SIRegisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    s32 i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < SI_MAX_CHAN; i++) {
        if (PollingHandler[i] == handler) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    for (i = 0; i < SI_MAX_CHAN; i++) {
        if (PollingHandler[i] == NULL) {
            PollingHandler[i] = handler;
            SIEnablePollingInterrupt(TRUE);
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL SIUnregisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    s32 i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < SI_MAX_CHAN; i++) {
        if (PollingHandler[i] == handler) {
            PollingHandler[i] = NULL;
            for (i = 0; i < SI_MAX_CHAN; i++) {
                if (PollingHandler[i] != NULL) {
                    break;
                }
            }
            if (i == SI_MAX_CHAN) {
                SIEnablePollingInterrupt(FALSE);
            }
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SIInit(void) {
    OSRegisterVersion(__SIVersion);

    Packet[0].chan = Packet[1].chan = Packet[2].chan = Packet[3].chan = CHAN_NONE;

    Si.poll = 0;

    SISetSamplingRate(0);

    while (__SIRegs[SI_COMCSR_IDX] & SI_COMCSR_TSTART_MASK) {
    }
    __SIRegs[SI_COMCSR_IDX] = SI_COMCSR_TCINT_MASK;

    __OSSetInterruptHandler(__OS_INTERRUPT_PI_SI, SIInterruptHandler);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK(__OS_INTERRUPT_PI_SI));

    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);
}

static BOOL SITransferStart(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes,
                            SICallback callback) {
    BOOL enabled;
    SICsr_u csr;
    u32 i;
    u32 n;
    u32 sr;

    enabled = OSDisableInterrupts();
    if (Si.chan != CHAN_NONE) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    sr = __SIRegs[SI_STATUS_IDX];
    sr &= SI_STATUS_ERROR_MASK >> (chan * 8);
    __SIRegs[SI_STATUS_IDX] = sr;

    Si.chan = chan;
    Si.callback = callback;
    Si.inputBytes = inputBytes;
    Si.input = input;

    n = (outputBytes + 3) / 4;
    for (i = 0; i < n; i++) {
        __SIRegs[SI_BUF_IDX + i] = ((u32*)output)[i];
    }

    csr.val = __SIRegs[SI_COMCSR_IDX];
    csr.f.tcint = 1;
    csr.f.tcintmsk = (callback != NULL) ? 1 : 0;
    csr.f.outlngth = (outputBytes == SI_MAX_TRANSFER_BYTES) ? 0 : outputBytes;
    csr.f.inlngth = (inputBytes == SI_MAX_TRANSFER_BYTES) ? 0 : inputBytes;
    csr.f.chan = chan;
    csr.f.tstart = 1;
    __SIRegs[SI_COMCSR_IDX] = csr.val;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sisr;
    int chanShift;

    enabled = OSDisableInterrupts();
    sisr = __SIRegs[SI_STATUS_IDX];
    chanShift = (3 - chan) * 8;
    sisr >>= chanShift;
    if ((sisr & SI_ERROR_NO_RESPONSE) != 0) {
        if ((Type[chan] & SI_ERROR_BUSY) == 0) {
            Type[chan] = SI_ERROR_NO_RESPONSE;
        }
    }
    OSRestoreInterrupts(enabled);
    return sisr;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[chan * SI_CHAN_STRIDE + SI_CHAN_OUTBUF] = command;
}

void SITransferCommands(void) {
    __SIRegs[SI_STATUS_IDX] = SI_SISR_WR_MASK;
}

u32 SISetXY(u32 x, u32 y) {
    BOOL enabled;
    u32 poll;

    poll = x << SI_POLL_X_SHIFT;
    poll |= y << SI_POLL_Y_SHIFT;
    enabled = OSDisableInterrupts();
    Si.poll &= ~SI_POLL_XY_MASK;
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[SI_POLL_IDX] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();
    poll >>= 24;
    en = poll & SI_POLL_EN_MASK;
    poll &= (en >> 4) | SI_POLL_XY_MASK | SI_POLL_EN_MASK;
    poll &= ~SI_POLL_XY_MASK;
    Si.poll &= ~(en >> 4);
    Si.poll |= poll;
    poll = Si.poll;
    SITransferCommands();
    __SIRegs[SI_POLL_IDX] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();
    poll >>= 24;
    poll &= SI_POLL_EN_MASK;
    poll = Si.poll & ~poll;
    __SIRegs[SI_POLL_IDX] = poll;
    Si.poll = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

static BOOL SIGetResponseRaw(s32 chan) {
    u32 sisr;

    sisr = SIGetStatus(chan);
    if ((sisr & SI_ERROR_RDST) != 0) {
        PollingResponse[chan][0] = __SIRegs[chan * SI_CHAN_STRIDE + SI_CHAN_INBUFH];
        PollingResponse[chan][1] = __SIRegs[chan * SI_CHAN_STRIDE + SI_CHAN_INBUFL];
        PollingPending[chan] = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL SIGetResponse(s32 chan, void* data) {
    BOOL enabled;
    BOOL ret;

    enabled = OSDisableInterrupts();
    SIGetResponseRaw(chan);
    ret = PollingPending[chan];
    PollingPending[chan] = FALSE;
    if (ret) {
        ((u32*)data)[0] = PollingResponse[chan][0];
        ((u32*)data)[1] = PollingResponse[chan][1];
    }

    OSRestoreInterrupts(enabled);
    return ret;
}

static void SITransferAlarmHandler(OSAlarm* alarm, OSContext* context) {
    s32 chan;
    struct SIPacket* packet;

    chan = alarm - Alarm;
    packet = &Packet[chan];
    if (packet->chan != CHAN_NONE) {
        if (SITransferStart(packet->chan, packet->output, packet->outputBytes, packet->input,
                            packet->inputBytes, packet->callback)) {
            packet->chan = CHAN_NONE;
        }
    }
}

BOOL SITransfer(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes,
                SICallback callback, OSTime delay) {
    BOOL enabled;
    struct SIPacket* packet;
    OSTime now;
    OSTime t;

    packet = &Packet[chan];
    enabled = OSDisableInterrupts();
    if (packet->chan != CHAN_NONE || Si.chan == chan) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    now = __OSGetSystemTime();
    if (delay == 0) {
        t = now;
    } else {
        t = TransferTime[chan] + delay;
    }

    if (now < t) {
        delay = t - now;
        OSSetAlarm(&Alarm[chan], delay, SITransferAlarmHandler);
    } else if (SITransferStart(chan, output, outputBytes, input, inputBytes, callback)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    packet->chan = chan;
    packet->output = output;
    packet->outputBytes = outputBytes;
    packet->input = input;
    packet->inputBytes = inputBytes;
    packet->callback = callback;
    packet->time = t;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

static inline void CallTypeAndStatusCallback(s32 chan, u32 type) {
    SITypeAndStatusCallback callback;
    int i;

    for (i = 0; i < SI_MAX_TYPE_CALLBACKS; i++) {
        callback = TypeCallback[chan][i];
        if (callback != NULL) {
            TypeCallback[chan][i] = NULL;
            callback(chan, type);
        }
    }
}

static void GetTypeCallback(s32 chan, u32 error, OSContext* context) {
    u32 type;
    u32 chanBit;
    BOOL fix;
    u32 id;

    Type[chan] &= ~SI_ERROR_BUSY;
    Type[chan] |= error;
    TypeTime[chan] = __OSGetSystemTime();

    type = Type[chan];

    chanBit = PAD_CHAN0_BIT >> chan;
    fix = __PADFixBits & chanBit;
    __PADFixBits &= ~chanBit;

    if ((error & SI_ERROR_MASK) != 0 || (type & SI_TYPE_MASK) != SI_TYPE_DOLPHIN ||
        (type & SI_GC_WIRELESS) == 0 || (type & SI_WIRELESS_IR) != 0) {
        OSSetWirelessID(chan, 0);
        CallTypeAndStatusCallback(chan, Type[chan]);
        return;
    }

    id = OSGetWirelessID(chan) << 8;

    if (fix && (id & SI_WIRELESS_FIX_ID) != 0) {
        PollingCommand[chan] = SI_WIRELESS_CMD | SI_WIRELESS_FIX_ID | (id & SI_WIRELESS_TYPE_ID);
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &PollingCommand[chan], SI_WIRELESS_CMD_LENGTH, &Type[chan],
                   SI_TYPE_REPLY_LENGTH, GetTypeCallback, 0);
        return;
    }

    if ((type & SI_WIRELESS_FIX_ID) != 0) {
        if ((id & SI_WIRELESS_TYPE_ID) != (type & SI_WIRELESS_TYPE_ID)) {
            if ((id & SI_WIRELESS_FIX_ID) == 0) {
                id = type & SI_WIRELESS_TYPE_ID;
                id |= SI_WIRELESS_FIX_ID;
                OSSetWirelessID(chan, id >> 8);
            }
            PollingCommand[chan] = SI_WIRELESS_CMD | id;
            Type[chan] = SI_ERROR_BUSY;
            SITransfer(chan, &PollingCommand[chan], SI_WIRELESS_CMD_LENGTH, &Type[chan],
                       SI_TYPE_REPLY_LENGTH, GetTypeCallback, 0);
            return;
        }
    } else if ((type & SI_WIRELESS_RECEIVED) != 0) {
        id = type & SI_WIRELESS_TYPE_ID;
        id |= SI_WIRELESS_FIX_ID;
        OSSetWirelessID(chan, id >> 8);
        PollingCommand[chan] = SI_WIRELESS_CMD | id;
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &PollingCommand[chan], SI_WIRELESS_CMD_LENGTH, &Type[chan],
                   SI_TYPE_REPLY_LENGTH, GetTypeCallback, 0);
        return;
    } else {
        OSSetWirelessID(chan, 0);
    }

    CallTypeAndStatusCallback(chan, Type[chan]);
}

u32 SIGetType(s32 chan) {
    static u32 cmdTypeAndStatus;
    BOOL enabled;
    u32 type;
    OSTime diff;

    enabled = OSDisableInterrupts();
    type = Type[chan];
    diff = __OSGetSystemTime() - TypeTime[chan];

    if ((Si.poll & (SI_POLL_EN0 >> chan)) != 0) {
        if (type != SI_ERROR_NO_RESPONSE) {
            TypeTime[chan] = __OSGetSystemTime();
            OSRestoreInterrupts(enabled);
            return type;
        }
        type = Type[chan] = SI_ERROR_BUSY;
    } else if (diff <= OSMillisecondsToTicks(50) && type != SI_ERROR_NO_RESPONSE) {
        OSRestoreInterrupts(enabled);
        return type;
    } else if (diff <= OSMillisecondsToTicks(75)) {
        Type[chan] = SI_ERROR_BUSY;
    } else {
        type = Type[chan] = SI_ERROR_BUSY;
    }

    TypeTime[chan] = __OSGetSystemTime();
    SITransfer(chan, &cmdTypeAndStatus, SI_TYPE_CMD_LENGTH, &Type[chan], SI_TYPE_REPLY_LENGTH,
               GetTypeCallback, OSMicrosecondsToTicks(65));
    OSRestoreInterrupts(enabled);
    return type;
}

u32 SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback) {
    BOOL enabled;
    u32 type;
    s32 i;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);
    if ((Type[chan] & SI_ERROR_BUSY) != 0) {
        for (i = 0; i < SI_MAX_TYPE_CALLBACKS; i++) {
            if (TypeCallback[chan][i] == callback) {
                break;
            }
            if (TypeCallback[chan][i] == NULL) {
                TypeCallback[chan][i] = callback;
                break;
            }
        }
    } else {
        callback(chan, type);
    }
    OSRestoreInterrupts(enabled);
    return type;
}

u32 SIDecodeType(u32 type) {
    u32 error;

    error = type & 0xFF;
    type &= 0xFFFFFF00;
    if ((error & SI_ERROR_NO_RESPONSE) != 0) {
        return SI_ERROR_NO_RESPONSE;
    }
    if ((error &
         (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_COLLISION | SI_ERROR_UNKNOWN)) != 0) {
        return SI_ERROR_UNKNOWN;
    }
    if (error != 0) {
        return SI_ERROR_BUSY;
    }

    if ((type & SI_TYPE_MASK) == SI_TYPE_N64) {
        switch (type & 0xFFFF0000) {
        case SI_N64_MIC:
        case SI_N64_KEYBOARD:
        case SI_GBA:
        case SI_N64_MOUSE:
        case SI_N64_CONTROLLER:
            return type & 0xFFFF0000;
        }
        return SI_ERROR_UNKNOWN;
    }

    if ((type & SI_TYPE_MASK) != SI_TYPE_DOLPHIN) {
        return SI_ERROR_UNKNOWN;
    }
    switch (type & 0xFFFF0000) {
    case SI_GC_STEERING:
    case SI_GC_CONTROLLER:
        return type & 0xFFFF0000;
    }
    if ((type & 0xFFE00000) == SI_GC_KEYBOARD) {
        return SI_GC_KEYBOARD;
    }
    if ((type & SI_GC_WIRELESS) != 0 && (type & SI_WIRELESS_IR) == 0) {
        if ((type & SI_GC_WAVEBIRD) == SI_GC_WAVEBIRD) {
            return SI_GC_WAVEBIRD;
        }
        if ((type & SI_WIRELESS_STATE) == 0) {
            return SI_GC_RECEIVER;
        }
    }
    if ((type & SI_GC_CONTROLLER) == SI_GC_CONTROLLER) {
        return SI_GC_CONTROLLER;
    }
    return SI_ERROR_UNKNOWN;
}

u32 SIProbe(s32 chan) {
    return SIDecodeType(SIGetType(chan));
}
