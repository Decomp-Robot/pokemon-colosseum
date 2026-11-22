/*
 * Field accessors for the battle information record.
 *
 * A missing record is treated as one filled with zeroes, so each getter
 * returns zero when it is passed a null pointer.
 */
#include "app/battle/battleInfo.h"

u16 zz_battle_get_battle_field(const BattleInfo *battleInfo)
{
    if (battleInfo == 0) {
        return 0;
    }
    return battleInfo->field;
}

u8 zz_battle_get_battle_style(const BattleInfo *battleInfo)
{
    if (battleInfo == 0) {
        return 0;
    }
    return battleInfo->style;
}

u8 zz_battle_get_battle_type(const BattleInfo *battleInfo)
{
    if (battleInfo == 0) {
        return 0;
    }
    return battleInfo->type;
}
