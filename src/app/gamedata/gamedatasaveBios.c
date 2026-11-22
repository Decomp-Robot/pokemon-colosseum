/*
 * Accessors for the game's save BIOS record: fetch the play-data block or
 * copy the system header. Both tolerate null pointers.
 */
#include "gamedata/gamedatasaveBios.h"

GamedataPlayData *gamedatasaveBiosGetplaydata(GamedataSaveBios *bios)
{
    if (bios == 0) {
        return 0;
    }
    return &bios->playData;
}

void gamedatasaveBiosSetSysHeadChecksum(GamedataSysHead *dst, GamedataSysHead *src)
{
    if (dst == 0) {
        return;
    }
    if (src == 0) {
        return;
    }
    *dst = *src;
}
