#ifndef GAMEDATA_SAVE_BIOS_H
#define GAMEDATA_SAVE_BIOS_H

#include "pokemon/types.h"

/* Play-data block held by the save BIOS record. */
typedef struct GamedataPlayData {
    u32 data[1];
} GamedataPlayData;

/* System-header record copied by gamedatasaveBiosSetSysHeadChecksum. */
typedef struct GamedataSysHead {
    u32 data[440];
} GamedataSysHead;

typedef struct GamedataSaveBios {
    u32 checksum;
    GamedataPlayData playData;
} GamedataSaveBios;

GamedataPlayData *gamedatasaveBiosGetplaydata(GamedataSaveBios *bios);
void gamedatasaveBiosSetSysHeadChecksum(GamedataSysHead *dst, GamedataSysHead *src);

#endif
