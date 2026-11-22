#ifndef APP_BATTLE_BATTLEINFO_H
#define APP_BATTLE_BATTLEINFO_H

#include "pokemon/types.h"

/*
 * Battle-information record shared by the battle data table and its
 * field accessors.
 */
typedef struct BattleInfo {
    u8 type;
    u8 style;
    u16 unk_02;
    u16 field;
    u8 unk_06[6];
    u32 bgmId;
    u8 unk_10[40];
} BattleInfo;

#endif
