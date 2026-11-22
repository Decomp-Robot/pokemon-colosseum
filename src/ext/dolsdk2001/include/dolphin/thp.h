#ifndef _DOLPHIN_THP_H
#define _DOLPHIN_THP_H

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef s16 THPCoeff;
    typedef float THPQuantTab[64];

    typedef struct _THPHuffmanTab
    {
        u8 quick[32];
        u8 increment[32];
        u8 *Vij;
        s32 maxCode[18];
        s32 valPtr[18];
        u8 Vij1;
        u8 pad[11];
    } THPHuffmanTab;

    typedef struct _THPComponent
    {
        u8 quantizationTableSelector;
        u8 DCTableSelector;
        u8 ACTableSelector;
        THPCoeff predDC;
    } THPComponent;

    typedef struct _THPFileInfo
    {
        THPQuantTab quantTabs[3];
        THPHuffmanTab huffmanTabs[4];
        THPComponent components[3];
        u16 xPixelSize;
        u16 yPixelSize;
        u16 MCUsPerRow;
        u16 decompressedY;
        u8 *c;
        u32 currByte;
        u32 cnt;
        u8 validHuffmanTabs;
        u8 RST;
        u16 nMCU;
        u16 currMCU;
        u8 *dLC[3];
    } THPFileInfo;

    /* Status codes returned by THPVideoDecode and its header parsers. */
    typedef enum _THPError
    {
        THP_ERR_NONE = 0,
        THP_ERR_BAD_SYNTAX = 3,
        THP_ERR_BAD_PRECISION = 10,
        THP_ERR_BAD_MARKER = 11,
        THP_ERR_BAD_COMPONENTS = 12,
        THP_ERR_BAD_HUFFMAN_TABLE = 15,
        THP_ERR_BAD_SAMPLING = 19,
        THP_ERR_NO_FILE = 25,
        THP_ERR_NO_WORK = 26,
        THP_ERR_NO_OUTPUT = 27,
        THP_ERR_LC_DISABLED = 28,
        THP_ERR_NOT_INITIALIZED = 29
    } THPError;

    BOOL THPInit(void);
    s32 THPVideoDecode(void *file, void *tileY, void *tileU, void *tileV, void *work);
    u32 THPAudioDecode(s16 *out, u8 *frame, s32 planar);

#ifdef __cplusplus
}
#endif

#endif
