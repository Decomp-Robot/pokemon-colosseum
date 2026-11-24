#include <dolphin.h>
#include <dolphin/exi.h>
#include <dolphin/hw_regs.h>
#include <string.h>

#include "__os.h"


#define EXI_DMA_ADDRESS_MASK 0x03FFFFE0

typedef struct ECB {
    EXICallback callback;
    EXICallback tcCallback;
    EXICallback extCallback;
    u32 flags;
    s32 length;
    u8* buffer;
    u32 device;
    u32 id;
    s32 probeTime;
    s32 lockCount;
    struct {
        u32 device;
        EXICallback callback;
    } locks[EXI_MAX_DEV];
} ECB;

static ECB Ecb[EXI_MAX_CHAN];

static u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext);
static BOOL __EXIProbe(s32 chan);

static void SetExiInterruptMask(s32 chan, ECB* ecb)
{
    ECB* ecb2 = &Ecb[EXI_CHAN_2];

    switch (chan) {
    case EXI_CHAN_0:
        if ((!ecb->callback && !ecb2->callback) || (ecb->flags & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_2_EXI);
        } else {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_2_EXI);
        }
        break;
    case EXI_CHAN_1:
        if (!ecb->callback || (ecb->flags & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXI);
        } else {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXI);
        }
        break;
    case EXI_CHAN_2:
        if (!__OSGetInterruptHandler(__OS_INTERRUPT_PI_DEBUG) || (ecb->flags & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
        } else {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
        }
        break;
    }
}

BOOL EXIImm(s32 chan, void* buffer, s32 length, u32 type, EXICallback callback)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();
    u32 data;
    s32 i;

    if ((ecb->flags & (EXI_STATE_DMA_BUSY | EXI_STATE_IMM_BUSY)) ||
        !(ecb->flags & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    ecb->tcCallback = callback;
    if (ecb->tcCallback) {
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_TC >> (chan * 3));
    }

    ecb->flags |= EXI_STATE_IMM_BUSY;
    if (type != EXI_READ) {
        data = 0;
        for (i = 0; i < length; i++) {
            data |= ((u8*)buffer)[i] << ((3 - i) * 8);
        }
        EXI_REG(chan, EXI_REG_DATA) = data;
    }

    ecb->buffer = buffer;
    ecb->length = (type != EXI_WRITE) ? length : 0;
    EXI_REG(chan, EXI_REG_CR) = (type << EXI_CR_RW_SHIFT) | EXI_CR_TSTART |
                                ((length - 1) << EXI_CR_TLEN_SHIFT);

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIImmEx(s32 chan, void* buffer, s32 length, u32 type)
{
    s32 size;

    while (length != 0) {
        size = (length < 4) ? length : 4;
        if (!EXIImm(chan, buffer, size, type, NULL)) {
            return FALSE;
        }
        if (!EXISync(chan)) {
            return FALSE;
        }
        buffer = (u8*)buffer + size;
        length -= size;
    }
    return TRUE;
}

BOOL EXIDma(s32 chan, void* buffer, s32 length, u32 type, EXICallback callback)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();

    if ((ecb->flags & (EXI_STATE_DMA_BUSY | EXI_STATE_IMM_BUSY)) ||
        !(ecb->flags & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    ecb->tcCallback = callback;
    if (ecb->tcCallback) {
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_TC >> (chan * 3));
    }

    ecb->flags |= EXI_STATE_DMA_BUSY;
    EXI_REG(chan, EXI_REG_MAR) = (u32)buffer & EXI_DMA_ADDRESS_MASK;
    EXI_REG(chan, EXI_REG_LENGTH) = length;
    EXI_REG(chan, EXI_REG_CR) = (type << EXI_CR_RW_SHIFT) | EXI_CR_DMA_START;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

static inline void CompleteTransfer(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    u8* buffer;
    u32 data;
    s32 length;
    s32 i;

    if (!(ecb->flags & (EXI_STATE_DMA_BUSY | EXI_STATE_IMM_BUSY))) {
        return;
    }
    if ((ecb->flags & EXI_STATE_IMM_BUSY) && (length = ecb->length) != 0) {
        buffer = ecb->buffer;
        data = EXI_REG(chan, EXI_REG_DATA);
        for (i = 0; i < length; i++) {
            *buffer++ = data >> ((3 - i) * 8);
        }
    }
    ecb->flags &= ~(EXI_STATE_DMA_BUSY | EXI_STATE_IMM_BUSY);
}

BOOL EXISync(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    BOOL result = FALSE;
    BOOL enabled;

    while (ecb->flags & EXI_STATE_SELECTED) {
        if (!(EXI_REG(chan, EXI_REG_CR) & EXI_CR_TSTART)) {
            enabled = OSDisableInterrupts();
            if (ecb->flags & EXI_STATE_SELECTED) {
                CompleteTransfer(chan);
                if (__OSGetDIConfig() != __OS_DI_CONFIG_NONE ||
                    ecb->length != 4 ||
                    (EXI_REG(chan, EXI_REG_CSR) & EXI_CSR_CLK_MASK) != 0 ||
                    EXI_REG(chan, EXI_REG_DATA) != EXI_USB_ADAPTER) {
                    result = TRUE;
                }
            }
            OSRestoreInterrupts(enabled);
            break;
        }
    }
    return result;
}

static u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext)
{
    u32 csr = EXI_REG(chan, EXI_REG_CSR);
    u32 oldCsr = csr;

    csr &= EXI_CSR_CONTROL_MASK;
    if (exi) {
        csr |= EXI_CSR_EXIINT;
    }
    if (tc) {
        csr |= EXI_CSR_TCINT;
    }
    if (ext) {
        csr |= EXI_CSR_EXTINT;
    }
    EXI_REG(chan, EXI_REG_CSR) = csr;
    return oldCsr;
}

EXICallback EXISetExiCallback(s32 chan, EXICallback callback)
{
    ECB* ecb = &Ecb[chan];
    EXICallback previousCallback;
    BOOL enabled = OSDisableInterrupts();

    previousCallback = ecb->callback;
    ecb->callback = callback;
    if (chan != EXI_CHAN_2) {
        SetExiInterruptMask(chan, ecb);
    } else {
        SetExiInterruptMask(EXI_CHAN_0, &Ecb[EXI_CHAN_0]);
    }
    OSRestoreInterrupts(enabled);
    return previousCallback;
}

static BOOL __EXIProbe(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    BOOL result;
    BOOL enabled;
    u32 csr;
    s32 tick;

    if (chan == EXI_CHAN_2) {
        return TRUE;
    }
    result = TRUE;
    enabled = OSDisableInterrupts();
    csr = EXI_REG(chan, EXI_REG_CSR);
    if (!(ecb->flags & EXI_STATE_ATTACHED)) {
        if (csr & EXI_CSR_EXTINT) {
            EXIClearInterrupts(chan, FALSE, FALSE, TRUE);
            __EXIProbeStartTime[chan] = ecb->probeTime = 0;
        }
        if (csr & EXI_CSR_EXTDET) {
            tick = (s32)(OSTicksToMilliseconds(OSGetTime()) / 100) + 1;
            if (__EXIProbeStartTime[chan] == 0) {
                __EXIProbeStartTime[chan] = tick;
            }
            if (tick - __EXIProbeStartTime[chan] < 3) {
                result = FALSE;
            }
        } else {
            __EXIProbeStartTime[chan] = ecb->probeTime = 0;
            result = FALSE;
        }
    } else if (!(csr & EXI_CSR_EXTDET) || (csr & EXI_CSR_EXTINT)) {
        __EXIProbeStartTime[chan] = ecb->probeTime = 0;
        result = FALSE;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

BOOL EXIProbe(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    BOOL result = __EXIProbe(chan);

    if (result && ecb->probeTime == 0) {
        u32 id;

        result = EXIGetID(chan, EXI_DEVICE_0, &id) ? TRUE : FALSE;
    }
    return result;
}

static inline BOOL __EXIAttach(s32 chan, EXICallback callback)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();

    if ((ecb->flags & EXI_STATE_ATTACHED) || !__EXIProbe(chan)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    ecb->extCallback = callback;
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXT >> (chan * 3));
    ecb->flags |= EXI_STATE_ATTACHED;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIAttach(s32 chan, EXICallback callback)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled;
    BOOL result;

    EXIProbe(chan);
    enabled = OSDisableInterrupts();
    if (ecb->probeTime == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    result = __EXIAttach(chan, callback);
    OSRestoreInterrupts(enabled);
    return result;
}

BOOL EXIDetach(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();

    if (!(ecb->flags & EXI_STATE_ATTACHED)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    if ((ecb->flags & EXI_STATE_LOCKED) && ecb->device == EXI_DEVICE_0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    ecb->flags &= ~EXI_STATE_ATTACHED;
    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0 >> (chan * 3));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXISelect(s32 chan, u32 device, u32 frequency)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();
    u32 csr;

    if ((ecb->flags & EXI_STATE_SELECTED) ||
        (chan != EXI_CHAN_2 &&
         ((device == EXI_DEVICE_0 && !(ecb->flags & EXI_STATE_ATTACHED) && !__EXIProbe(chan)) ||
          !(ecb->flags & EXI_STATE_LOCKED) || ecb->device != device))) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    ecb->flags |= EXI_STATE_SELECTED;
    csr = EXI_REG(chan, EXI_REG_CSR);
    csr &= EXI_CSR_SELECT_MASK;
    csr |= ((1 << device) << EXI_CSR_CS_SHIFT) | (frequency << EXI_CSR_CLK_SHIFT);
    EXI_REG(chan, EXI_REG_CSR) = csr;

    if (ecb->flags & EXI_STATE_ATTACHED) {
        switch (chan) {
        case EXI_CHAN_0:
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXT);
            break;
        case EXI_CHAN_1:
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXT);
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIDeselect(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    u32 csr;
    BOOL enabled = OSDisableInterrupts();

    if (!(ecb->flags & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    ecb->flags &= ~EXI_STATE_SELECTED;
    csr = EXI_REG(chan, EXI_REG_CSR);
    EXI_REG(chan, EXI_REG_CSR) = csr & EXI_CSR_SELECT_MASK;

    if (ecb->flags & EXI_STATE_ATTACHED) {
        switch (chan) {
        case EXI_CHAN_0:
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXT);
            break;
        case EXI_CHAN_1:
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXT);
            break;
        }
    }
    OSRestoreInterrupts(enabled);

    if (chan != EXI_CHAN_2 && (csr & EXI_CSR_CS0)) {
        return __EXIProbe(chan) ? TRUE : FALSE;
    }
    return TRUE;
}

static void EXIIntrruptHandler(s16 intr, OSContext* context)
{
    s32 chan = (intr - __OS_INTERRUPT_EXI_0_EXI) / 3;
    ECB* ecb = &Ecb[chan];
    EXICallback callback;
    OSContext exceptionContext;

    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    callback = ecb->callback;
    if (callback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

static void TCIntrruptHandler(s16 intr, OSContext* context)
{
    s32 chan = (intr - __OS_INTERRUPT_EXI_0_TC) / 3;
    ECB* ecb = &Ecb[chan];
    EXICallback callback;
    OSContext exceptionContext;

    __OSMaskInterrupts(OS_INTERRUPTMASK(intr));
    EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
    callback = ecb->tcCallback;
    if (callback) {
        ecb->tcCallback = NULL;
        CompleteTransfer(chan);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

static void EXTIntrruptHandler(s16 intr, OSContext* context)
{
    s32 chan = (intr - __OS_INTERRUPT_EXI_0_EXT) / 3;
    ECB* ecb;
    EXICallback callback;
    OSContext exceptionContext;

    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0 >> (chan * 3));
    EXI_REG(chan, EXI_REG_CSR) = 0;
    ecb = &Ecb[chan];
    callback = ecb->extCallback;
    ecb->flags &= ~EXI_STATE_ATTACHED;
    if (callback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        ecb->extCallback = NULL;
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

void EXIInit(void)
{
    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI);
    EXI_REG(EXI_CHAN_0, EXI_REG_CSR) = 0;
    EXI_REG(EXI_CHAN_1, EXI_REG_CSR) = 0;
    EXI_REG(EXI_CHAN_2, EXI_REG_CSR) = 0;
    EXI_REG(EXI_CHAN_0, EXI_REG_CSR) = EXI_CSR_ROMDIS;

    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_0_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_0_TC, TCIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_0_EXT, EXTIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_1_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_1_TC, TCIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_1_EXT, EXTIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_2_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_2_TC, TCIntrruptHandler);

    if (OSGetConsoleType() & OS_CONSOLE_DEVELOPMENT) {
        __EXIProbeStartTime[EXI_CHAN_0] = __EXIProbeStartTime[EXI_CHAN_1] = 0;
        Ecb[EXI_CHAN_0].probeTime = Ecb[EXI_CHAN_1].probeTime = 0;
        __EXIProbe(EXI_CHAN_0);
        __EXIProbe(EXI_CHAN_1);
    }
}

BOOL EXILock(s32 chan, u32 device, EXICallback callback)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 i;

    if (ecb->flags & EXI_STATE_LOCKED) {
        if (callback) {
            for (i = 0; i < ecb->lockCount; i++) {
                if (ecb->locks[i].device == device) {
                    OSRestoreInterrupts(enabled);
                    return FALSE;
                }
            }
            ecb->locks[ecb->lockCount].callback = callback;
            ecb->locks[ecb->lockCount].device = device;
            ecb->lockCount++;
        }
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    ecb->flags |= EXI_STATE_LOCKED;
    ecb->device = device;
    SetExiInterruptMask(chan, ecb);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIUnlock(s32 chan)
{
    ECB* ecb = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();
    EXICallback callback;

    if (!(ecb->flags & EXI_STATE_LOCKED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    ecb->flags &= ~EXI_STATE_LOCKED;
    SetExiInterruptMask(chan, ecb);
    if (ecb->lockCount > 0) {
        callback = ecb->locks[0].callback;
        if (--ecb->lockCount > 0) {
            memmove(&ecb->locks[0], &ecb->locks[1], ecb->lockCount * sizeof(ecb->locks[0]));
        }
        callback(chan, NULL);
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 EXIGetState(s32 chan)
{
    ECB* ecb = &Ecb[chan];

    return ecb->flags;
}

static void UnlockedHandler(s32 chan, OSContext* context)
{
    u32 id;

    EXIGetID(chan, EXI_DEVICE_0, &id);
}

s32 EXIGetID(s32 chan, u32 device, u32* id)
{
    ECB* ecb = &Ecb[chan];
    s32 startTime;
    BOOL err;
    u32 command;

    if (chan < EXI_CHAN_2 && device == EXI_DEVICE_0) {
        if (!__EXIProbe(chan)) {
            return 0;
        }
        if (ecb->probeTime == __EXIProbeStartTime[chan]) {
            *id = ecb->id;
            return ecb->probeTime;
        }
        if (!__EXIAttach(chan, NULL)) {
            return 0;
        }
        startTime = __EXIProbeStartTime[chan];
    }

    err = !EXILock(chan, device, (chan < EXI_CHAN_2 && device == EXI_DEVICE_0) ? UnlockedHandler : NULL);
    if (!err) {
        err = !EXISelect(chan, device, EXI_FREQ_1M);
        if (!err) {
            command = 0;
            err |= !EXIImm(chan, &command, 2, EXI_WRITE, NULL);
            err |= !EXISync(chan);
            err |= !EXIImm(chan, id, 4, EXI_READ, NULL);
            err |= !EXISync(chan);
            err |= !EXIDeselect(chan);
        }
        EXIUnlock(chan);
    }

    if (chan < EXI_CHAN_2 && device == EXI_DEVICE_0) {
        BOOL enabled;

        EXIDetach(chan);
        enabled = OSDisableInterrupts();
        err |= (__EXIProbeStartTime[chan] != startTime);
        if (!err) {
            ecb->id = *id;
            ecb->probeTime = startTime;
        }
        OSRestoreInterrupts(enabled);
        if (err) {
            return 0;
        }
        return ecb->probeTime;
    }
    if (err) {
        return 0;
    }
    return 1;
}
