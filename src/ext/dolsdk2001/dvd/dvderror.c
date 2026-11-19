#include <dolphin.h>

#include "__dvd.h"
#include "__os.h"

// Software markers from the DVD command engine. They reserve the top two
// values of the extended-SRAM error byte.
#define DVD_FATAL_ERROR_CODE 0x01234567
#define DVD_TIMEOUT_ERROR_CODE 0x01234568
#define DVD_SRAM_FATAL_ERROR 0xFF
#define DVD_SRAM_TIMEOUT_ERROR 254

// Extended SRAM packs a DI status code with one of 30 error numbers.
#define DVD_SRAM_MAX_STATUS 6
#define DVD_SRAM_ERRORS_PER_STATUS 30
#define DVD_SRAM_DIDNT_MATCH 29
#define DVD_SRAM_EXTENDED_ERROR 17

#define DVD_ERROR_STATUS_SHIFT 24
#define DVD_ERROR_CODE_MASK 0x00FFFFFF
#define DVD_ERROR_EXTENDED_FIRST 0x00100000
#define DVD_ERROR_EXTENDED_LAST 0x00100008

// Low-24-bit error codes reported by the DI Request Error command. The matching
// table index is the error-number component stored in extended SRAM.
static u32 ErrorTable[18] = {
    0x00000000, // no error
    0x00023A00, // medium not present
    0x00062800, // medium may have changed
    0x00030200, // no seek complete
    0x00031100, // unrecovered read error
    0x00052000, // invalid command opcode
    0x00052001, // audio buffer not set
    0x00052100, // logical block address out of range
    0x00052400, // invalid field in command packet
    0x00052401, // invalid audio command
    0x00052402, // configuration period error
    0x000B5A01, // operator medium removal request
    0x00056300, // end of the user area on the track
    0x00020401, // disc ID not read
    0x00020400, // motor stopped
    0x00040800, // transfer protocol error
    0x00100007, // extended-range error code 7, reported as error number 16
    0x00000000, // no error
};

static u8 ErrorCode2Num(u32 error) {
    u32 i;

    for (i = 0; i < sizeof(ErrorTable) / sizeof(ErrorTable[0]); i++) {
        if (error == ErrorTable[i]) {
            return i;
        }
    }
    if (error >= DVD_ERROR_EXTENDED_FIRST && error <= DVD_ERROR_EXTENDED_LAST) {
        return DVD_SRAM_EXTENDED_ERROR;
    }
    return DVD_SRAM_DIDNT_MATCH;
}

static inline u8 Convert(u32 error) {
    u32 statusCode;
    u32 errorCode;
    u8 errorNum;

    if (error == DVD_FATAL_ERROR_CODE) return DVD_SRAM_FATAL_ERROR;
    if (error == DVD_TIMEOUT_ERROR_CODE) return DVD_SRAM_TIMEOUT_ERROR;
    statusCode = error >> DVD_ERROR_STATUS_SHIFT;
    errorCode = error & DVD_ERROR_CODE_MASK;
    errorNum = ErrorCode2Num(errorCode);
    if (statusCode >= DVD_SRAM_MAX_STATUS) statusCode = DVD_SRAM_MAX_STATUS;
    return errorNum + statusCode * DVD_SRAM_ERRORS_PER_STATUS;
}

void __DVDStoreErrorCode(u32 error) {
    u8 code;
    OSSramEx * sram;

    code = Convert(error);
    sram = __OSLockSramEx();
    sram->dvdErrorCode = code;
    __OSUnlockSramEx(TRUE);
}
