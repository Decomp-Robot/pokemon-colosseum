#include <dolphin.h>
#include <dolphin/ai.h>
#include <dolphin/dvd.h>
#include <dolphin/os.h>

#include "__os.h"

#define APPLOADER_LOAD_ADDR 0x81300000
#define APPLOADER_DISK_OFFSET 0x2440

extern unsigned long RebootCodeReserved AT_ADDRESS(0x817FFFF8);
extern unsigned long RebootCode AT_ADDRESS(0x817FFFFC);
extern void* RebootSavedRegionStart AT_ADDRESS(0x812FDFF0);
extern void* RebootSavedRegionEnd AT_ADDRESS(0x812FDFEC);
extern unsigned char RebootRequestFlag AT_ADDRESS(0x800030E2);

static void* SaveStart;
static void* SaveEnd;
static volatile int Prepared;

typedef struct ApploaderHeader {
    char date[16];
    unsigned long entry;
    unsigned long size;
    unsigned long trailerSize;
    unsigned long reserved;
} ApploaderHeader;

static ApploaderHeader Header ATTRIBUTE_ALIGN(32);

static asm void Run(void (*entry)(void)) {
    nofralloc
    sync
    isync
    mtlr r3
    blr
}

static void PrepareCallback(long result, DVDCommandBlock* block) {
    Prepared = TRUE;
}

static inline int IsStreamEnabled(void) {
    if (DVDGetCurrentDiskID()->streaming) {
        return TRUE;
    }
    return FALSE;
}

void __OSReboot(unsigned long resetCode, int forceMenu) {
    OSContext context;
    DVDCommandBlock streamBlock;
    DVDCommandBlock headerBlock;
    DVDCommandBlock apploaderBlock;
    OSTime startTime;
    unsigned long appSize;
    unsigned long appOffset;

    OSDisableInterrupts();
    RebootCode = 0;
    RebootCodeReserved = 0;
    RebootRequestFlag = TRUE;
    RebootSavedRegionStart = SaveStart;
    RebootSavedRegionEnd = SaveEnd;

    OSClearContext(&context);
    OSSetCurrentContext(&context);

    DVDInit();
    DVDSetAutoInvalidation(TRUE);
    DVDResume();

    Prepared = FALSE;
    __DVDPrepareResetAsync(PrepareCallback);

    __OSMaskInterrupts(0xFFFFFFE0);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_DI);
    OSEnableInterrupts();

    startTime = OSGetTime();
    while (Prepared != TRUE) {
        if (!DVDCheckDisk() || (OSGetTime() - startTime) > OSSecondsToTicks(1)) {
            __OSDoHotReset(RebootCode);
        }
    }

    if (!__OSIsGcam && IsStreamEnabled()) {
        AISetStreamVolLeft(0);
        AISetStreamVolRight(0);
        DVDCancelStreamAsync(&streamBlock, NULL);

        startTime = OSGetTime();
        while (DVDGetCommandBlockStatus(&streamBlock)) {
            if (!DVDCheckDisk() || (OSGetTime() - startTime) > OSSecondsToTicks(1)) {
                __OSDoHotReset(RebootCode);
            }
        }

        AISetStreamPlayState(AI_STREAM_STOP);
    }

    DVDReadAbsAsyncPrio(&headerBlock, &Header, sizeof(Header), APPLOADER_DISK_OFFSET, NULL, 0);

    startTime = OSGetTime();
    while (DVDGetCommandBlockStatus(&headerBlock)) {
        if (!DVDCheckDisk() || (OSGetTime() - startTime) > OSSecondsToTicks(1)) {
            __OSDoHotReset(RebootCode);
        }
    }

    appOffset = Header.size + sizeof(Header);
    appSize = OSRoundUp32B(Header.trailerSize);

    DVDReadAbsAsyncPrio(&apploaderBlock, (void*)APPLOADER_LOAD_ADDR, appSize, appOffset + APPLOADER_DISK_OFFSET, NULL, 0);

    startTime = OSGetTime();
    while (DVDGetCommandBlockStatus(&apploaderBlock)) {
        if (!DVDCheckDisk() || (OSGetTime() - startTime) > OSSecondsToTicks(1)) {
            __OSDoHotReset(RebootCode);
        }
    }

    ICInvalidateRange((void*)APPLOADER_LOAD_ADDR, appSize);
    OSDisableInterrupts();
    ICFlashInvalidate();
    Run((void (*)(void))APPLOADER_LOAD_ADDR);
}
