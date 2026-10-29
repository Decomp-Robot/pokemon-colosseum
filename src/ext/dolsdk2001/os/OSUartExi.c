#include <dolphin/exi.h>
#include <dolphin/os.h>

static u32 serDev;
static u32 serChan;
static u32 serEnabled;
static u32 serInitialized;

// None of the functions in this file are declared in a public header; the
// serial console is only reachable through the OS exec/message machinery.
int InitializeUART();
// UNUSED: the game never calls this; dead-stripped at link.
int ReadUARTN();
int WriteUARTN(void* buf, u32 len);

static inline int QueueLength(void) {
    unsigned long cmd;

    if (EXISelect(serDev, serChan, 3) == 0) {
        return -1;
    }
    cmd = 0x20010000;
    EXIImm(serDev, &cmd, 4, 1, 0);
    EXISync(serDev);
    EXIImm(serDev, &cmd, 1, 0, 0);
    EXISync(serDev);
    EXIDeselect(serDev);
    return 0x10 - (cmd >> 0x18);
}

int InitializeUART() {
    if (serInitialized == 0xA5FF005A) {
        return 0;
    }
    if (!(OSGetConsoleType() & 0x10000000)) {
        serEnabled = 0;
        return 2;
    }
    serEnabled = 0xA5FF005A;
    serDev = 0;
    serChan = 1;
    return 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
int ReadUARTN() {
    return 4;
}

int WriteUARTN(void* buf, u32 len) {
    unsigned long cmd;
    long xLen;
    BOOL enabled;
    int qLen;
    char* ptr;
    int error;

    if (serEnabled != 0xA5FF005A) {
        return 2;
    }

    enabled = OSDisableInterrupts();
    if (EXILock(serDev, serChan, 0) == 0) {
        OSRestoreInterrupts(enabled);
        return 0;
    }
    else {
        ptr = (char*)buf;
    }

    while ((u32)ptr - (u32)buf < len) {
        if (*(s8*)ptr == 0xA) {
            *ptr = 0xD;
        }
        ptr++;
    }
    error = 0;
    cmd = 0xA0010000;

    while (len != 0) {
        qLen = QueueLength();
        if (qLen < 0) {
            error = 3;
            break;
        }

        if ((qLen >= 0xC) || (qLen >= len)) {
            if (EXISelect(serDev, serChan, 3) == 0) {
                error = 3;
                break;
            }

            EXIImm(serDev, &cmd, 4, 1, 0);
            EXISync(serDev);

            while ((qLen != 0) && (len != 0)) {
                if ((qLen < 4) && (qLen < len)) {
                    break;
                }

                xLen = len < 4 ? (long)len : 4;

                EXIImm(serDev, buf, xLen, 1, 0);
                (char*)buf += xLen;
                len -= xLen;
                qLen -= xLen;
                EXISync(serDev);
            }
            EXIDeselect(serDev);
        }
    }
    EXIUnlock(serDev);
    OSRestoreInterrupts(enabled);
    return error;
}
