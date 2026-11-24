#ifndef _DOLPHIN_EXI_H_
#define _DOLPHIN_EXI_H_

#include <dolphin/os/OSContext.h>

typedef void (*EXICallback)(s32 chan, OSContext *context);

typedef enum {
    EXI_CHAN_0,
    EXI_CHAN_1,
    EXI_CHAN_2,
    EXI_MAX_CHAN
} EXIChannel;

typedef enum {
    EXI_DEVICE_0,
    EXI_DEVICE_1,
    EXI_DEVICE_2,
    EXI_MAX_DEV
} EXIDevice;

typedef enum {
    EXI_FREQ_1M,
    EXI_FREQ_2M,
    EXI_FREQ_4M,
    EXI_FREQ_8M,
    EXI_FREQ_16M,
    EXI_FREQ_32M
} EXIFrequency;

#define EXI_USB_ADAPTER 0x01010000

typedef enum {
    EXI_STATE_DMA_BUSY = 1 << 0,
    EXI_STATE_IMM_BUSY = 1 << 1,
    EXI_STATE_SELECTED = 1 << 2,
    EXI_STATE_ATTACHED = 1 << 3,
    EXI_STATE_LOCKED = 1 << 4
} EXIState;

typedef enum {
    EXI_READ,
    EXI_WRITE,
    EXI_READ_WRITE
} EXITransferType;

EXICallback EXISetExiCallback(s32 chan, EXICallback callback);

void EXIInit(void);
BOOL EXILock(s32 chan, u32 device, EXICallback callback);
BOOL EXIUnlock(s32 chan);
BOOL EXISelect(s32 chan, u32 device, u32 frequency);
BOOL EXIDeselect(s32 chan);
BOOL EXIImm(s32 chan, void *buffer, s32 length, u32 type, EXICallback callback);
BOOL EXIImmEx(s32 chan, void *buffer, s32 length, u32 type);
BOOL EXIDma(s32 chan, void *buffer, s32 length, u32 type, EXICallback callback);
BOOL EXISync(s32 chan);
BOOL EXIProbe(s32 chan);
s32 EXIProbeEx(s32 chan);
BOOL EXIAttach(s32 chan, EXICallback callback);
BOOL EXIDetach(s32 chan);
u32 EXIGetState(s32 chan);
s32 EXIGetID(s32 chan, u32 device, u32* id);
void EXIProbeReset(void);

#endif
