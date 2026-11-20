#include <dolphin.h>
#include <dolphin/exi.h>
#include <dolphin/hw_regs.h>
#include <dolphin/os.h>
#include <limits.h>
#include <string.h>

#include "__os.h"

#define FATAL_WIDTH 640
#define FATAL_HEIGHT 480
#define FATAL_WORK_AREA (18 * 1024 * 1024)
#define FATAL_ARENA_ALIGNMENT 32
#define FATAL_TEXT_H_MARGIN 48
#define FATAL_TEXT_Y 100
#define FATAL_RESET_TIMEOUT_MS 1000

#define FONT_CELL_SIZE 24
#define FONT_TILE_SIZE 8
#define FONT_TEXEL_BITS 4
#define FONT_TILE_MASK (FONT_TILE_SIZE - 1u)
#define FONT_TEXEL_MASK ((1 << FONT_TEXEL_BITS) - 1)
#define FONT_STRIDE_UNIT 4

#define VIDEO_LUMA_BLACK 16
#define VIDEO_LUMA_MAX 235.0f
#define VIDEO_CHROMA_MIN 16.0f
#define VIDEO_CHROMA_MAX 240.0f
#define VIDEO_CHROMA_CENTER 128.0f

#define EXI_IPL_CHANNEL 0
#define EXI_IPL_ROM_DEVICE 1
#define EXI_SHARED_IRQ_CHANNEL 2

typedef struct OSFatalParam {
    GXColor fg;
    GXColor bg;
    const char* msg;
} OSFatalParam;

static OSFatalParam FatalParam;
static OSContext FatalContext;

// prototypes
static void Halt(void);

static void ScreenClear(u8* xfb, u16 xfbW, u16 xfbH, GXColor yuv) {
    int i;
    int j;
    u8* ptr;

    ptr = xfb;
    for (i = 0; i < xfbH; i++) {
        for (j = 0; j < xfbW; j += 2) {
            *ptr++ = yuv.r;
            *ptr++ = yuv.g;
            *ptr++ = yuv.r;
            *ptr++ = yuv.b;
        }
    }
}

static void ScreenReport(u8* xfb, u16 xfbW, u16 xfbH, GXColor yuv, s32 x, s32 y, s32 leading, const char* string) {
    u8* ptr;
    s32 width;
    u32 i;
    u32 j;
    u32 image[FONT_CELL_SIZE * FONT_CELL_SIZE * FONT_TEXEL_BITS / (sizeof(u32) * CHAR_BIT)];
    u32 k;
    u32 l;
    u8 Y;
    u32 pixel;
    s32 col;

nextLine:
    if (xfbH - FONT_CELL_SIZE >= y) {
        ptr = xfb + ((x + (y * xfbW)) * VI_DISPLAY_PIX_SZ);
        col = x;

        while (*string != 0) {
            if (*string == '\n') {
                string++;
                y += leading;
                goto nextLine;
            }

            if (xfbW - FATAL_TEXT_H_MARGIN < col) {
                y += leading;
                goto nextLine;
            }

            for (i = 0; i < FONT_CELL_SIZE; i++) {
                j = (i & FONT_TILE_MASK) + ((i / FONT_TILE_SIZE) * FONT_CELL_SIZE);
                image[j] = 0;
                image[j + FONT_TILE_SIZE] = 0;
                image[j + 2 * FONT_TILE_SIZE] = 0;
            }

            string = OSGetFontTexel((char*)string, image, 0, FONT_CELL_SIZE / FONT_STRIDE_UNIT, &width);

            for (i = 0; i < FONT_CELL_SIZE; i++) {
                j = (i & FONT_TILE_MASK) + ((i / FONT_TILE_SIZE) * FONT_CELL_SIZE);

                for (k = 0; k < FONT_CELL_SIZE; k++) {
                    l = j + (k & ~FONT_TILE_MASK);

                    Y = (image[l] >> ((FONT_TILE_MASK - (k & FONT_TILE_MASK)) * FONT_TEXEL_BITS)) & FONT_TEXEL_MASK;
                    if (Y != 0) {
                        Y = (((yuv.r * (Y * (UCHAR_MAX - VIDEO_LUMA_BLACK))) / UCHAR_MAX) / FONT_TEXEL_MASK) + VIDEO_LUMA_BLACK;
                        pixel = k + (i * xfbW);
                        ptr[pixel * VI_DISPLAY_PIX_SZ] = Y;

                        if ((col + k) & 1) {
                            ptr[pixel * VI_DISPLAY_PIX_SZ - 1] = yuv.g;
                            ptr[pixel * VI_DISPLAY_PIX_SZ + 1] = yuv.b;
                        } else {
                            ptr[pixel * VI_DISPLAY_PIX_SZ - 1] = yuv.b;
                            ptr[pixel * VI_DISPLAY_PIX_SZ + 1] = yuv.g;
                        }
                    }
                }
            }

            ptr += width * VI_DISPLAY_PIX_SZ;
            col += width;
        }
    }
}

static void ConfigureVideo(u16 xfbW, u16 xfbH) {
    GXRenderModeObj mode;
    mode.fbWidth = xfbW;
    mode.efbHeight = FATAL_HEIGHT;
    mode.xfbHeight = xfbH;
    mode.viXOrigin = (VI_MAX_WIDTH_NTSC - FATAL_WIDTH) / 2;
    mode.viWidth = FATAL_WIDTH;
    mode.viHeight = xfbH;

    switch (VIGetTvFormat()) {
    case VI_MPAL:
    case VI_NTSC:
        if (__VIRegs[VI_CLOCK_SEL] & VI_CLOCK_54MHZ) {
            mode.viTVmode = VI_TVMODE_NTSC_PROG;
            mode.viYOrigin = 0;
            mode.xFBmode = VI_XFBMODE_SF;
        } else {
            mode.viTVmode = VI_TVMODE_NTSC_INT;
            mode.viYOrigin = 0;
            mode.xFBmode = VI_XFBMODE_DF;
        }
        break;
    case VI_EURGB60:
        mode.viTVmode = VI_TVMODE_EURGB60_INT;
        mode.viYOrigin = 0;
        mode.xFBmode = VI_XFBMODE_DF;
        break;
    case VI_PAL:
        mode.viTVmode = VI_TVMODE_PAL_INT;
        mode.viYOrigin = (VI_MAX_HEIGHT_PAL - FATAL_HEIGHT) / 2;
        mode.xFBmode = VI_XFBMODE_DF;
        break;
    }

    VIConfigure(&mode);
    VIConfigurePan(0, 0, FATAL_WIDTH, FATAL_HEIGHT);
}

static GXColor RGB2YUV(GXColor rgb) {
    f32 Y;
    f32 Cb;
    f32 Cr;
    GXColor yuv;

    Y  = (0.257f * rgb.r + 0.504f * rgb.g + 0.098f * rgb.b) + VIDEO_LUMA_BLACK + 0.5f;
    Cb = (-0.148f * rgb.r - 0.291f * rgb.g + 0.439f * rgb.b) + VIDEO_CHROMA_CENTER + 0.5f;
    Cr = (0.439f * rgb.r - 0.368f * rgb.g - 0.071f * rgb.b) + VIDEO_CHROMA_CENTER + 0.5f;

    yuv.r = (Y > VIDEO_LUMA_MAX) ? VIDEO_LUMA_MAX : (Y < VIDEO_LUMA_BLACK) ? VIDEO_LUMA_BLACK : Y;
    yuv.g = (Cb > VIDEO_CHROMA_MAX) ? VIDEO_CHROMA_MAX : (Cb < VIDEO_CHROMA_MIN) ? VIDEO_CHROMA_MIN : Cb;
    yuv.b = (Cr > VIDEO_CHROMA_MAX) ? VIDEO_CHROMA_MAX : (Cr < VIDEO_CHROMA_MIN) ? VIDEO_CHROMA_MIN : Cr;
    yuv.a = 0;

    return yuv;
}

void OSFatal(GXColor fg, GXColor bg, const char* msg) {
    OSBootInfo* bootInfo;
    u32 count;
    OSTime t;

    bootInfo = OSPhysicalToCached(0);
    OSDisableInterrupts();
    OSDisableScheduler();
    OSClearContext(&FatalContext);
    OSSetCurrentContext(&FatalContext);
    __OSStopAudioSystem();
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    VIInit();
    VISetBlack(TRUE);
    VIFlush();
    OSEnableInterrupts();

    count = VIGetRetraceCount();
    do {} while ((s32)(VIGetRetraceCount() - count) < 1);

    t = OSGetTime();
    while (!CallResetFunctions(FALSE) && OSGetTime() - t < OSMillisecondsToTicks(FATAL_RESET_TIMEOUT_MS)) {}

    OSDisableInterrupts();
    CallResetFunctions(TRUE);
    __OSSetExceptionHandler(__OS_EXCEPTION_DECREMENTER, OSDefaultExceptionHandler);
    GXAbortFrame();
    OSSetArenaLo(OSPhysicalToCached(FATAL_WORK_AREA));
    OSSetArenaHi(bootInfo->FSTLocation);

    FatalParam.fg = fg;
    FatalParam.bg = bg;
    FatalParam.msg = msg;
    OSSwitchFiber((u32)&Halt, (u32)OSGetArenaHi());
}

static void Halt(void) {
    u32 count;
    OSFontHeader* fontData;
    void* xfb;
    u32 len;
    OSFatalParam* fp;

    OSEnableInterrupts();
    fp = &FatalParam;
    len = strlen(fp->msg) + 1;
    fp->msg = memmove(OSAllocFromArenaLo(len, FATAL_ARENA_ALIGNMENT), fp->msg, len);

    EXISetExiCallback(EXI_IPL_CHANNEL, NULL);
    EXISetExiCallback(EXI_SHARED_IRQ_CHANNEL, NULL);

    while (!EXILock(EXI_IPL_CHANNEL, EXI_IPL_ROM_DEVICE, NULL)) {
        while ((__EXIRegs[EXI_CR(EXI_IPL_CHANNEL)] & EXI_CR_TSTART) == EXI_CR_TSTART) {}
        EXIDeselect(EXI_IPL_CHANNEL);
        EXIUnlock(EXI_IPL_CHANNEL);
    }
    EXIUnlock(EXI_IPL_CHANNEL);

    while ((__EXIRegs[EXI_CR(EXI_IPL_CHANNEL)] & EXI_CR_TSTART) == EXI_CR_TSTART) {}

    if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS) {
        fontData = OSAllocFromArenaLo(OS_FONT_DATA_SIZE_SJIS, FATAL_ARENA_ALIGNMENT);
    } else {
        fontData = OSAllocFromArenaLo(OS_FONT_DATA_SIZE_ANSI, FATAL_ARENA_ALIGNMENT);
    }
    OSLoadFont(fontData, OSGetArenaLo());

    xfb = OSAllocFromArenaLo(FATAL_WIDTH * FATAL_HEIGHT * VI_DISPLAY_PIX_SZ, FATAL_ARENA_ALIGNMENT);
    ScreenClear(xfb, FATAL_WIDTH, FATAL_HEIGHT, RGB2YUV(fp->bg));
    VISetNextFrameBuffer(xfb);
    ConfigureVideo(FATAL_WIDTH, FATAL_HEIGHT);
    VIFlush();

    count = VIGetRetraceCount();
    do {} while ((s32)(VIGetRetraceCount() - count) < 2);

    ScreenReport(xfb, FATAL_WIDTH, FATAL_HEIGHT, RGB2YUV(fp->fg), FATAL_TEXT_H_MARGIN, FATAL_TEXT_Y, fontData->leading, fp->msg);
    DCFlushRange(xfb, FATAL_WIDTH * FATAL_HEIGHT * VI_DISPLAY_PIX_SZ);
    VISetBlack(FALSE);
    VIFlush();

    count = VIGetRetraceCount();
    do {} while ((s32)(VIGetRetraceCount() - count) < 1);

    OSDisableInterrupts();
    OSReport("%s\n", fp->msg);
    PPCHalt();
}
