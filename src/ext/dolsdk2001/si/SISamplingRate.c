#include <dolphin/os.h>
#include <dolphin/si.h>
#include <dolphin/vi.h>
#include <dolphin/hw_regs.h>

typedef struct {
    u16 x;
    u8 y;
} XY;

enum { SI_SAMPLING_RATE_MAX = 11 };

static XY XYNTSC[SI_SAMPLING_RATE_MAX + 1] = {
    {246, 2}, {15, 18}, {30, 9},  {44, 6},  {52, 5},  {65, 4},
    {87, 3},  {87, 3},  {87, 3},  {131, 2}, {131, 2}, {131, 2},
};

static XY XYPAL[SI_SAMPLING_RATE_MAX + 1] = {
    {296, 2}, {15, 21}, {29, 11}, {45, 7},  {52, 6},  {63, 5},
    {78, 4},  {104, 3}, {104, 3}, {104, 3}, {104, 3}, {156, 2},
};

#define SI_SAMPLING_RATE_DEFAULT 0
#define VI_CLOCK_54MHZ 0x0001

static u32 SamplingRate;

void SISetSamplingRate(u32 msec) {
    XY *timing;
    BOOL interrupt;
    u32 clockMultiplier;
    
    if (msec > SI_SAMPLING_RATE_MAX) {
        msec = SI_SAMPLING_RATE_MAX;
    }

    interrupt = OSDisableInterrupts();
    SamplingRate = msec;
    switch (VIGetTvFormat()) {
    case VI_NTSC:
    case VI_MPAL:
    case VI_EURGB60:
        timing = XYNTSC;
        break;
    case VI_PAL:
        timing = XYPAL;
        break;
    default:
        OSReport("SISetSamplingRate: unknown TV format. Use default.");
        msec = SI_SAMPLING_RATE_DEFAULT;
        timing = XYNTSC;
        break;
    }

    if (__VIRegs[VI_CLOCK_SEL] & VI_CLOCK_54MHZ) {
        clockMultiplier = 2;
    } else {
        clockMultiplier = 1;
    }

    SISetXY(timing[msec].x * clockMultiplier, timing[msec].y);
    OSRestoreInterrupts(interrupt);
}

void SIRefreshSamplingRate(void) {
    SISetSamplingRate(SamplingRate);
}
