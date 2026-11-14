#include <dolphin.h>
#include <dolphin/gx.h>
#include <dolphin/G2D.h>
#include "fake_tgmath.h"

static G2DGlob glob;

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

static inline void FillSection(G2DLayer* layer, s8* aSortBuffer, s32* nScanLine, s32 nEvent, s16* nIdx,
        s32* nL, s32* nR, f32* rLeft, f32* rRight, f32 rStep0, f32 rStep1, s32 nMapX, s32 nMapY) {
    long nHMask;
    long nVMask;
    long nI;
    long nJ;
    long nK;
    long nM;
    s16 nMaterial;
    s16* pAddr;

    nHMask = (1 << layer->nHS) - 1;
    nVMask = (1 << layer->nVS) - 1;
    if (layer->nBPI == 1) {
        u8 nTile;
        if (layer->bWrap) {
            for (; *nScanLine <= nEvent; (*nScanLine)++) {
                nJ = *nScanLine - 1;
                nK = (nVMask & (nJ + nMapY)) << layer->nHS;
                for (nI = *nL; nI <= *nR; nI++) {
                    nTile = ((u8*)layer->map)[nK + (nHMask & (nI + nMapX))];
                    nMaterial = layer->tileDesc[nTile].nMaterial;
                    pAddr = (s16*)&layer->matDesc[nMaterial].nReserved;
                    pAddr[1]++;

                    if (*pAddr != *nIdx) {
                        *nIdx += 2;
                        *(s16*)&aSortBuffer[*pAddr] = *pAddr - *nIdx;
                    }
                    *(s16*)&aSortBuffer[*nIdx] = nTile;
                    *nIdx += 2;
                    aSortBuffer[(*nIdx)++] = nI;
                    aSortBuffer[(*nIdx)++] = nJ;
                    *pAddr = *nIdx;
                }
                *rLeft += rStep0;
                *rRight += rStep1;
                *nL = floor(*rLeft);
                *nR = floor(*rRight);
            }
        }
        else {
            for (; *nScanLine <= nEvent; (*nScanLine)++) {
                nJ = *nScanLine - 1 + nMapY;
                if (nJ < 0) {
                    nK = 0;
                }
                else if ( nJ > nVMask ) {
                    nK = nVMask << layer->nHS;
                }
                else {
                    nK = nJ << layer->nHS;
                }
                nM = *nR + nMapX;
                for (nI = *nL + nMapX; nI <= nM; nI++) {
                    if ( nI < 0 ) {
                        nTile = ((u8*)layer->map)[nK];
                    }
                    else if ( nI > nHMask ) {
                        nTile = ((u8*)layer->map)[nK + nHMask];
                    }
                    else {
                        nTile = ((u8*)layer->map)[nK + nI];
                    }

                    nMaterial = layer->tileDesc[nTile].nMaterial;
                    pAddr = (s16*)&layer->matDesc[nMaterial].nReserved;
                    pAddr[1]++;

                    if (*pAddr != *nIdx) {
                        *nIdx += 2;
                        *(s16*)&aSortBuffer[*pAddr] = *pAddr - *nIdx;
                    }
                    *(s16*)&aSortBuffer[*nIdx] = nTile;
                    *nIdx += 2;
                    aSortBuffer[(*nIdx)++] = nI - nMapX;
                    aSortBuffer[(*nIdx)++] = nJ - nMapY;
                    *pAddr = *nIdx;
                }
                *rLeft += rStep0;
                *rRight += rStep1;
                *nL = floor(*rLeft);
                *nR = floor(*rRight);
            }
        }
    }
    else {
        u16 nTile;
        if (layer->bWrap) {
            for (; *nScanLine <= nEvent; (*nScanLine)++) {
                nJ = *nScanLine - 1;
                nK = (nVMask & (nJ + nMapY)) << layer->nHS;
                for (nI = *nL; nI <= *nR; nI++) {
                    nTile = ((u16*)layer->map)[nK + (nHMask & (nI + nMapX))];
                    nMaterial = layer->tileDesc[nTile].nMaterial;
                    pAddr = (s16*)&layer->matDesc[nMaterial].nReserved;
                    pAddr[1]++;

                    if (*pAddr != *nIdx) {
                        *nIdx += 2;
                        *(s16*)&aSortBuffer[*pAddr] = *pAddr - *nIdx;
                    }
                    *(s16*)&aSortBuffer[*nIdx] = nTile;
                    *nIdx += 2;
                    aSortBuffer[(*nIdx)++] = nI;
                    aSortBuffer[(*nIdx)++] = nJ;
                    *pAddr = *nIdx;
                }
                *rLeft += rStep0;
                *rRight += rStep1;
                *nL = floor(*rLeft);
                *nR = floor(*rRight);
            }
        }
        else {
            for (; *nScanLine <= nEvent; (*nScanLine)++) {
                nJ = *nScanLine - 1 + nMapY;
                if (nJ < 0) {
                    nK = 0;
                }
                else if ( nJ > nVMask ) {
                    nK = nVMask << layer->nHS;
                }
                else {
                    nK = nJ << layer->nHS;
                }
                nM = *nR + nMapX;
                for (nI = *nL + nMapX; nI <= nM; nI++) {
                    if ( nI < 0 ) {
                        nTile = ((u16*)layer->map)[nK];
                    }
                    else if ( nI > nHMask ) {
                        nTile = ((u16*)layer->map)[nK + nHMask];
                    }
                    else {
                        nTile = ((u16*)layer->map)[nK + nI];
                    }

                    nMaterial = layer->tileDesc[nTile].nMaterial;
                    pAddr = (s16*)&layer->matDesc[nMaterial].nReserved;
                    pAddr[1]++;

                    if (*pAddr != *nIdx) {
                        *nIdx += 2;
                        *(s16*)&aSortBuffer[*pAddr] = *pAddr - *nIdx;
                    }
                    *(s16*)&aSortBuffer[*nIdx] = nTile;
                    *nIdx += 2;
                    aSortBuffer[(*nIdx)++] = nI - nMapX;
                    aSortBuffer[(*nIdx)++] = nJ - nMapY;
                    *pAddr = *nIdx;
                }
                *rLeft += rStep0;
                *rRight += rStep1;
                *nL = floor(*rLeft);
                *nR = floor(*rRight);
            }
        }
    }
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
