#include <dolphin/types.h>
#include <dolphin/os.h>
#include <dolphin/si.h>
#include "__os.h"

// The Sep 2002 era pad.h provided this struct and these button macros, but
// records the PADStatus layout and callback-era API used by this SDK revision.
typedef struct PADStatus
{
    /*0x00*/ u16 button;
    /*0x02*/ s8 stickX;
    /*0x03*/ s8 stickY;
    /*0x04*/ s8 substickX;
    /*0x05*/ s8 substickY;
    /*0x06*/ u8 triggerLeft;
    /*0x07*/ u8 triggerRight;
    /*0x08*/ u8 analogA;
    /*0x09*/ u8 analogB;
    /*0x0A*/ s8 err;
} PADStatus;

#define PAD_BUTTON_LEFT  (1 << 0)
#define PAD_BUTTON_RIGHT (1 << 1)
#define PAD_BUTTON_DOWN  (1 << 2)
#define PAD_BUTTON_UP    (1 << 3)
#define PAD_TRIGGER_Z    (1 << 4)
#define PAD_TRIGGER_R    (1 << 5)
#define PAD_TRIGGER_L    (1 << 6)
#define PAD_BUTTON_A     (1 << 8)
#define PAD_BUTTON_B     (1 << 9)
#define PAD_BUTTON_X     (1 << 10)
#define PAD_BUTTON_Y     (1 << 11)
#define PAD_BUTTON_MENU  (1 << 12)
#define PAD_BUTTON_START (1 << 12)

// SI functions used by this revision but absent from si.h.
extern void SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback);
extern s32 SIRegisterPollingHandler(void (*handler)(s32, OSContext *));
extern s32 SIUnregisterPollingHandler(void (*handler)(s32, OSContext *));
extern s32 zz_800CF708_(void);
extern s32 zz_800CF728_(int arg);
extern s32 zz_800D05A4_(s32 chan, void * response); // SIGetResponse in target SI revision
extern s32 zz_800D034C_(void);                     // SITransferCommands
/* The target SI revision exposes these calls under address-only identities in
 * the map. PADInit's call at 0x800AB05C resolves to the 24-byte wrapper at
 * 0x800D104C (which refreshes the SI sampling rate); the SI command setter is
 * the 0x800D0338 helper. */
extern void zz_800D0338_(s32 chan, u32 command);
extern void zz_800D104C_(void);
#define SISetCommand zz_800D0338_
#define SIRefreshSamplingRate zz_800D104C_
extern u32 lbl_8047AA58;

static BOOL PADOnReset(BOOL final);

void SamplingHandler(s32 chan, OSContext * context);
s32 PADSetSamplingCallback(void (*callback)(void));
BOOL __PADDisableRecalibration(BOOL disable);

static void SPEC0_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]);
static void SPEC1_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]);
static void SPEC2_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]);
#define PAD_ALL \
    (PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT | PAD_BUTTON_DOWN | PAD_BUTTON_UP | PAD_TRIGGER_Z | PAD_TRIGGER_R \
     | PAD_TRIGGER_L | PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y | PAD_BUTTON_MENU | 0x2000 | 0x0080)

u16 __OSWirelessPadFixMode : 0x800030E0;
volatile u8 __OSDisableRecalibrationFlag : 0x800030E3;

char *__PADVersion = "<< Dolphin SDK - PAD\trelease build: Sep  5 2002 05:34:02 (0x2301) >>";
s32 ResettingChan = 32;
u32 padBits = 0xF0000000;
u32 AnalogMode = 0x300;
u32 Spec = 5;
void (*MakeStatus)(s32, struct PADStatus *, u32 *) = SPEC2_MakeStatus;
u32 OriginCmd = 0x41000000;
u32 CmdCalibrate = 0x42000000;
static OSResetFunctionInfo PADResetFunctionInfo = { PADOnReset, 127, NULL, NULL };

u32 __PADSpec;
BOOL recalibrated;
void (*samplingCallback)(void);
u32 ProbingBits;
u32 CheckingBits;
u32 WaitingBits;
u32 RecalibrateBits;
u32 ResettingBits;
u32 EnabledBits;
int Initialized;

u32 padProbeCmd[4];
struct PADStatus Origin[4];
static u32 Type[4];

static void DoReset(void);
static inline void PADEnable(s32 chan);
void UpdateOrigin(s32 chan);
void PADOriginCallback(s32 chan, u32 error, OSContext *context);
void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context);
void PADProbeCallback(s32 chan, u32 error, OSContext *context);
void PADTypeAndStatusCallback(s32 chan, u32 type);
void PADReceiveCheckCallback(s32 chan, u32 type);
s32 PADReset(u32 mask);
BOOL PADRecalibrate(u32 mask);
BOOL PADInit(void);
s32 PADRead(struct PADStatus * status);
void PADControlAllMotors(const u32 * commandArray);
void PADSetSpec(u32 spec);
static void SPEC0_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]);
static void SPEC1_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]);
static void SPEC2_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]);
void PADSetAnalogMode(u32 mode);
void SamplingHandler(s32 chan, OSContext * context);
s32 PADSetSamplingCallback(void (*callback)(void));
BOOL __PADDisableRecalibration(BOOL disable);
BOOL PADRecalibrate(u32 mask);
static inline u16 GetWirelessID(s32 chan);
static inline void SetWirelessID(s32 chan, u16 id);
static inline void ProbeWireless(s32 chan);
static inline void PADControlMotor(s32 chan, u32 command);
static inline void PADDisableChan(s32 chan);
static inline void PADFixCallback(s32 unused, u32 error, OSContext * context);
static inline void PADResetCallback(s32 unused, u32 error, OSContext * context);
static inline u32 PADGetSpec(void);
static inline int PADGetType(s32 chan, u32 * type);
static inline void PADSetSamplingRate(unsigned long msec);


static inline void DoReset(void)
{
    u32 chanBit;

    ResettingChan = __cntlzw(ResettingBits);
    if (ResettingChan != 32) {
        chanBit = 0x80000000 >> ResettingChan;
        ResettingBits &= ~chanBit;
        memset(&Origin[ResettingChan], 0, 12);
        SIGetTypeAsync(ResettingChan, PADTypeAndStatusCallback);
    }
}

static inline void PADEnable(s32 chan)
{
    u32 cmd;
    u32 chanBit;
    u32 probe[2];

    chanBit = 0x80000000 >> chan;
    EnabledBits |= chanBit;
    zz_800D05A4_(chan, probe);
    SISetCommand(chan, AnalogMode | 0x400000);
    SIEnablePolling(EnabledBits);
}

void UpdateOrigin(s32 chan)
{
    struct PADStatus * origin = &Origin[chan];
    unsigned long bit = 0x80000000 >> chan;

    switch (AnalogMode & 0x00000700u) {
    case 0x00000000u:
    case 0x00000500u:
    case 0x00000600u:
    case 0x00000700u:
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000100u:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000200u:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        break;
    case 0x00000300u:
        break;
    case 0x00000400u:
        break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (padBits & bit) {
        if ((s8)origin->stickX > 64) {
            if ((SIGetType(chan) & 0xFFFF0000) == 0x09000000) {
                origin->stickX = 0;
            }
        }
    }
}

void PADOriginCallback(s32 chan, u32 error, OSContext *context)
{
    if (!(error & 0xF)) {
        UpdateOrigin(ResettingChan);
        PADEnable(ResettingChan);
    }
    DoReset();
}

static inline void PADDisableChan(s32 chan)
{
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = 0x80000000 >> chan;
    SIDisablePolling(chanBit);
    EnabledBits &= ~chanBit;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;
    ProbingBits &= ~chanBit;
    OSSetWirelessID(chan, 0);
    OSRestoreInterrupts(enabled);
}

void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context)
{
    if (!(EnabledBits & (0x80000000 >> chan))) {
        return;
    }
    if (!(error & 0xF)) {
        UpdateOrigin(chan);
    }
    if (error & 0x08) {
        PADDisableChan(chan);
    }
}

void PADProbeCallback(s32 chan, u32 error, OSContext *context)
{
    if (!(error & 0xF)) {
        PADEnable(ResettingChan);
        WaitingBits |= 0x80000000 >> ResettingChan;
    }
    DoReset();
}

void PADTypeAndStatusCallback(s32 chan, u32 type)
{
    u32 chanBit;
    u32 recalibrate;
    s32 rc;
    u32 error;

    chanBit = 0x80000000 >> ResettingChan;
    recalibrate = RecalibrateBits & chanBit;
    RecalibrateBits &= ~chanBit;
    rc = 1;
    error = type & 0xF;

    if (error & 0xF) {
        DoReset();
        return;
    }

    type &= ~0xFF;
    Type[ResettingChan] = type;

    if ((type & SI_TYPE_MASK) != SI_TYPE_GC || !(type & SI_GC_STANDARD)) {
        DoReset();
        return;
    }

    if (Spec < 2) {
        PADEnable(ResettingChan);
        DoReset();
        return;
    }

    if (!(type & SI_GC_WIRELESS) || (type & SI_WIRELESS_IR)) {
        if (recalibrate) {
            rc = SITransfer(ResettingChan, &CmdCalibrate, 3, &Origin[ResettingChan], 10,
                            (SICallback)PADOriginCallback, 0);
        } else {
            rc = SITransfer(ResettingChan, &OriginCmd, 1, &Origin[ResettingChan], 10,
                            (SICallback)PADOriginCallback, 0);
        }
    } else if ((type & 0x00100000) && !(type & 0x00080000) && !(type & 0x00040000)) {
        if (type & 0x40000000) {
            rc = SITransfer(ResettingChan, &OriginCmd, 1, &Origin[ResettingChan], 10,
                            (SICallback)PADOriginCallback, 0);
        } else {
            rc = SITransfer(ResettingChan, &padProbeCmd[ResettingChan], 3, &Origin[ResettingChan], 8,
                            (SICallback)PADProbeCallback, 0);
        }
    }
    if (!rc) {
        ProbingBits |= chanBit;
        DoReset();
        return;
    }
}

void PADReceiveCheckCallback(s32 chan, u32 type)
{
    unsigned long error;
    unsigned long bit = 0x80000000 >> chan;

    if (!(EnabledBits & bit)) {
        return;
    }
    WaitingBits &= ~bit;
    CheckingBits &= ~bit;
    error = type & 0xFF;
    type &= ~0xFF;
    if (!(error & 0xF) && (type & SI_GC_WIRELESS) && (type & SI_WIRELESS_FIX_ID)
        && (type & SI_WIRELESS_RECEIVED) && !(type & SI_WIRELESS_IR)
        && (type & SI_WIRELESS_CONT_MASK) == SI_WIRELESS_CONT && !(type & SI_WIRELESS_LITE)) {
        SITransfer(chan, &OriginCmd, 1, &Origin[chan], 10, (SICallback)PADOriginUpdateCallback, 0);
        return;
    }
    PADDisableChan(chan);
}

s32 PADReset(u32 mask)
{
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();
    mask |= ProbingBits;
    ProbingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);

    ResettingBits |= mask;
    disableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;

    if (Spec == 4) {
        RecalibrateBits |= mask;
    }

    SIDisablePolling(disableBits);

    if (ResettingChan == 32) {
        DoReset();
    }

    OSRestoreInterrupts(enabled);
    return 1;
}

BOOL PADRecalibrate(u32 mask)
{
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();
    mask |= ProbingBits;
    ProbingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);

    ResettingBits |= mask;
    disableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;

    if (!(__OSDisableRecalibrationFlag & 0x40)) {
        RecalibrateBits |= mask;
    }

    SIDisablePolling(disableBits);

    if (ResettingChan == 32) {
        DoReset();
    }

    OSRestoreInterrupts(enabled);
    return 1;
}

BOOL PADInit(void)
{
    s32 chan;
    s64 time;

    if (Initialized) {
        return TRUE;
    }
    OSRegisterVersion(__PADVersion);
    if (__PADSpec) {
        PADSetSpec(__PADSpec);
    }
    Initialized = 1;
    if (lbl_8047AA58 != 0) {
        time = OSGetTime();
        __OSWirelessPadFixMode = (u16)((((time) & 0xffff) + ((time >> 16) & 0xffff)
            + ((time >> 32) & 0xffff) + ((time >> 48) & 0xffff)) & 0x3fffu);
        RecalibrateBits = 0xF0000000;
    }
    padProbeCmd[0] = (0x4D << 24) | ((__OSWirelessPadFixMode & 0x3FFFu) << 8);
    padProbeCmd[1] = (0x4D << 24) | (1 << 22) | ((__OSWirelessPadFixMode & 0x3FFFu) << 8);
    padProbeCmd[2] = (0x4D << 24) | (2 << 22) | ((__OSWirelessPadFixMode & 0x3FFFu) << 8);
    padProbeCmd[3] = (0x4D << 24) | (3 << 22) | ((__OSWirelessPadFixMode & 0x3FFFu) << 8);
    SIRefreshSamplingRate();
    OSRegisterResetFunction(&PADResetFunctionInfo);
    return PADReset(0xF0000000);
}

s32 PADRead(struct PADStatus * status)
{
    BOOL enabled;
    unsigned long mask;
    s32 i;
    s32 result;
    u32 probe[2];
    unsigned long chanBit;

    enabled = OSDisableInterrupts();
    result = 0;
    for (i = 0; i < 4; ++i, ++status) {
        mask = 0x80000000 >> i;
        if ((ProbingBits & mask) != 0) {
            PADReset(0);
            status->err = -2;
            memset(status, 0, 10);
        } else if ((ResettingBits & mask) != 0 || ResettingChan == i) {
            status->err = -2;
            memset(status, 0, 10);
        } else if ((EnabledBits & mask) == 0) {
            status->err = -1;
            memset(status, 0, 10);
        } else if (zz_800CF728_(i) != 0) {
            status->err = -3;
            memset(status, 0, 10);
        } else if ((SIGetStatus(i) & 0x8) != 0) {
            zz_800D05A4_(i, probe);
            if ((WaitingBits & mask) != 0) {
                status->err = 0;
                memset(status, 0, 10);
                if ((CheckingBits & mask) == 0) {
                    CheckingBits |= mask;
                    SIGetTypeAsync(i, PADReceiveCheckCallback);
                }
            } else {
                PADDisableChan(i);
                status->err = -1;
                memset(status, 0, 10);
            }
        } else {
            if (!(SIGetType(i) & 0x20000000)) {
                result |= mask;
            }
            if (zz_800D05A4_(i, probe) == 0) {
                status->err = -3;
                memset(status, 0, 10);
            } else if ((probe[0] & 0x80000000) != 0) {
                status->err = -3;
                memset(status, 0, 10);
            } else {
                MakeStatus(i, status, probe);
                if (status->button & 0x2000) {
                    status->err = -3;
                    memset(status, 0, 10);
                    SITransfer(i, &OriginCmd, 1, &Origin[i], 10, (SICallback)PADOriginUpdateCallback, 0);
                } else {
                    status->err = 0;
                    status->button &= ~0x80;
                }
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}

void PADControlAllMotors(const u32 * commandArray)
{
    BOOL enabled;
    s32 chan;
    BOOL commit;
    u32 id;

    enabled = OSDisableInterrupts();
    commit = FALSE;
    for (chan = 0; chan < 4; ++chan, ++commandArray) {
        if ((EnabledBits & 0x80000000 >> chan) == 0) {
            continue;
        }
        if (SIGetType(chan) & 0x20000000) {
            continue;
        }
        id = *commandArray;
        if (Spec < 2 && id == 2) {
            id = 0;
        }
        SISetCommand(chan, AnalogMode | 0x400000 | (id & 3));
        commit = TRUE;
    }
    if (commit) {
        zz_800D034C_();
    }
    OSRestoreInterrupts(enabled);
}

void PADSetSpec(u32 spec)
{
    __PADSpec = 0;
    switch (spec) {
    case 0:
        MakeStatus = SPEC0_MakeStatus;
        break;
    case 1:
        MakeStatus = SPEC1_MakeStatus;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        MakeStatus = SPEC2_MakeStatus;
        break;
    }
    Spec = spec;
}


static inline s8 ClampS8(s8 var, s8 org)
{
    if (0 < org) {
        s8 min = (s8)(-128 + org);
        if (var < min) {
            var = min;
        }
    } else if (org < 0) {
        s8 max = (s8)(127 + org);
        if (max < var) {
            var = max;
        }
    }
    return var -= org;
}

static inline u8 ClampU8(u8 var, u8 org)
{
    if (var < org) {
        var = org;
    }
    return var -= org;
}

static void SPEC0_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0008) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0001) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_START : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static void SPEC1_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0080) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0200) ? PAD_BUTTON_START : 0;

    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);

    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];

    status->analogA = 0;
    status->analogB = 0;

    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static void SPEC2_MakeStatus(s32 chan, struct PADStatus * status, u32 data[2]) {
    struct PADStatus * origin;

    status->button = (u16)((data[0] >> 16) & PAD_ALL);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);

    switch (AnalogMode & 0x00000700)
    {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0f) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0f) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    case 0x00000300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1] >> 0);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x00000400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    origin = &Origin[chan];
    status->stickX = ClampS8(status->stickX, origin->stickX);
    status->stickY = ClampS8(status->stickY, origin->stickY);
    status->substickX = ClampS8(status->substickX, origin->substickX);
    status->substickY = ClampS8(status->substickY, origin->substickY);
    status->triggerLeft = ClampU8(status->triggerLeft, origin->triggerLeft);
    status->triggerRight = ClampU8(status->triggerRight, origin->triggerRight);
}

void PADSetAnalogMode(u32 mode)
{
    BOOL enabled;
    u32 bits;

    enabled = OSDisableInterrupts();
    AnalogMode = mode << 8;
    bits = EnabledBits;
    EnabledBits &= ~bits;
    WaitingBits &= ~bits;
    CheckingBits &= ~bits;
    SIDisablePolling(bits);
    OSRestoreInterrupts(enabled);
}

static inline BOOL PADSync(void)
{
    return ResettingBits == 0 && ResettingChan == 32 && !zz_800CF708_();
}

static BOOL PADOnReset(BOOL final)
{
    BOOL sync;

    if (samplingCallback != 0) {
        PADSetSamplingCallback(NULL);
    }
    if (!final) {
        sync = PADSync();
        if (!recalibrated && sync) {
            recalibrated = PADRecalibrate(0xF0000000);
            return FALSE;
        }
        return sync;
    } else {
        recalibrated = FALSE;
    }
    return TRUE;
}

void SamplingHandler(s32 chan, OSContext * context)
{
    OSContext exceptionContext;

    if (samplingCallback != 0) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        samplingCallback();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

s32 PADSetSamplingCallback(void (*callback)(void))
{
    s32 old_callback;
    s32 ret;

    old_callback = (s32)samplingCallback;
    samplingCallback = callback;
    if (callback != 0) {
        ret = SIRegisterPollingHandler(SamplingHandler);
    } else {
        ret = SIUnregisterPollingHandler(SamplingHandler);
    }
    return old_callback;
}

BOOL __PADDisableRecalibration(BOOL disable)
{
    BOOL enabled;
    BOOL result;

    enabled = OSDisableInterrupts();
    if (__OSDisableRecalibrationFlag & 0x40) {
        result = TRUE;
    } else {
        result = FALSE;
    }
    __OSDisableRecalibrationFlag &= (u8)~0x40;
    if (disable) {
        __OSDisableRecalibrationFlag |= 0x40;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline u16 GetWirelessID(s32 chan)
{
    struct OSSramEx * sram;
    u16 id;

    sram = __OSLockSramEx();
    id = sram->wirelessPadID[chan];
    __OSUnlockSramEx(0);
    return id;
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline void SetWirelessID(s32 chan, u16 id)
{
    struct OSSramEx * sram = __OSLockSramEx();

    if (sram->wirelessPadID[chan] != id) {
        sram->wirelessPadID[chan] = id;
        __OSUnlockSramEx(1);
        return;
    }
    __OSUnlockSramEx(0);
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline void ProbeWireless(s32 chan)
{
    unsigned long chanBit;
    unsigned long cmd;
    unsigned long data[2];
    unsigned long type;

    chanBit = 0x80000000 >> chan;
    EnabledBits |= chanBit;
    ProbingBits |= chanBit;
    zz_800D05A4_(chan, data);
    type = Type[chan];
    if (!(type & 0x02000000)) {
        cmd = (chan << 0xE) | 0x4D0000 | (__OSWirelessPadFixMode & 0x3FFF);
    } else if (((type & 0xC0000) + 0xFFFC0000) == 0) {
        cmd = 0x500000;
    } else {
        cmd = (type & 0x70000) + 0x440000;
    }
    SISetCommand(chan, cmd);
    SIEnablePolling(EnabledBits);
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline void PADControlMotor(s32 chan, u32 command)
{
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = 0x80000000 >> chan;
    if ((EnabledBits & chanBit) && !(ProbingBits & chanBit) && !(Type[chan] & 0x20000000)) {
        if (Spec < 2 && command == 2) {
            command = 0;
        }
        SISetCommand(chan, (0x40 << 16) | AnalogMode | (command & (0x00000001 | 0x00000002)));
        zz_800D034C_();
    }
    OSRestoreInterrupts(enabled);
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline void PADFixCallback(s32 unused, u32 error, OSContext * context)
{
    unsigned long type;
    unsigned long id;
    unsigned long frame;

    if (!(error & 0xF)) {
        type = Type[ResettingChan];
        id = (GetWirelessID(ResettingChan) << 8);
        if (!(type & 0x100000) || ((id & 0xCFFF00u) != (type & 0xCFFF00))) {
            DoReset();
            return;
        }
        if ((type & 0x40000000) && !(type & 0x80000) && !(type & 0x40000)) {
            frame = 0x41000000;
            SITransfer(ResettingChan, &frame, 1, &Origin[ResettingChan], 0xA, (SICallback)PADOriginCallback, 0);
            return;
        }
        frame = (ResettingChan << 0x16) | 0x4D000000 | (__OSWirelessPadFixMode << 8) & 0x3FFF00u;
        SITransfer(ResettingChan, &frame, 3, &Origin[ResettingChan], 8, (SICallback)PADProbeCallback, 0);
        return;
    }
    DoReset();
}

// UNUSED: the game never calls this; dead-stripped at link.
// (the wireless fix-bits handling of the newer revision is not present in this build)
static inline void PADResetCallback(s32 unused, u32 error, OSContext * context)
{
    unsigned long type;
    unsigned long id;
    unsigned long recalibrate;
    unsigned long chanBit;
    unsigned long frame;

    type = Type[ResettingChan];
    chanBit = 0x80000000 >> ResettingChan;
    recalibrate = RecalibrateBits & chanBit;
    RecalibrateBits &= ~chanBit;
    if ((error & 0xF) || (((type & 0x18000000) + 0xF8000000) != 0)) {
        SetWirelessID(ResettingChan, 0);
        DoReset();
        return;
    }
    if (Spec < 2) {
        PADEnable(ResettingChan);
        DoReset();
        return;
    }
    if (!(type & 0x80000000) || (type & 0x04000000)) {
        SetWirelessID(ResettingChan, 0);
        if (!(type & 0x01000000)) {
            DoReset();
            return;
        }
        if (recalibrate != 0) {
            frame = 0x42000000;
            SITransfer(ResettingChan, &frame, 3, &Origin[ResettingChan], 0xA, (SICallback)PADOriginCallback, 0);
            return;
        }
        frame = 0x41000000;
        SITransfer(ResettingChan, &frame, 1, &Origin[ResettingChan], 0xA, (SICallback)PADOriginCallback, 0);
        return;
    }
    id = (GetWirelessID(ResettingChan) << 8);
    if (type & 0x100000) {
        if ((id & 0xCFFF00) != (type & 0xCFFF00)) {
            if (!(id & 0x100000)) {
                id = (type & 0xCFFF00);
                id |= 0x100000;
                SetWirelessID(ResettingChan, (u16)(id >> 8) & 0xFFFFFF);
            }
            frame = id | 0x4E000000;
            SITransfer(ResettingChan, &frame, 3, &Type[ResettingChan], 3, (SICallback)PADFixCallback, 0);
            return;
        }
        if ((type & 0x40000000) && !(type & 0x80000) && !(type & 0x40000)) {
            frame = 0x41000000;
            SITransfer(ResettingChan, &frame, 1, &Origin[ResettingChan], 0xA, (SICallback)PADOriginCallback, 0);
            return;
        }
    }
    if (recalibrate != 0) {
        frame = 0x42000000;
        SITransfer(ResettingChan, &frame, 3, &Origin[ResettingChan], 0xA, (SICallback)PADOriginCallback, 0);
        return;
    }
    frame = 0x41000000;
    SITransfer(ResettingChan, &frame, 1, &Origin[ResettingChan], 0xA, (SICallback)PADOriginCallback, 0);
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline u32 PADGetSpec(void)
{
    return Spec;
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline int PADGetType(s32 chan, u32 * type)
{
    unsigned long chanBit;

    *type = Type[chan];
    chanBit = 0x80000000 >> chan;
    if (ResettingBits & chanBit || ResettingChan == chan || !(EnabledBits & chanBit)) {
        return 0;
    }
    return 1;
}

// UNUSED: the game never calls this; dead-stripped at link.
static inline void PADSetSamplingRate(unsigned long msec)
{
    if (msec > 0xB) {
        msec = 0xB;
    }
    SISetSamplingRate(msec);
}
