#include "app/battle/battleMove.h"
#include "pokemon/types.h"

#define DAMAGE_MESSAGE 60516

enum MoveId {
    MOVE_ID_354 = 354,
    MOVE_ID_355 = 355,
    MOVE_ID_356 = 356,
    MOVE_ID_357 = 357,
    MOVE_ID_NONE = 0xFFFF,
};

extern u32 zz_801F1C18_(u32, void *, u32 *, u32, u32);
extern u16 zz_80236520_(void *, void *);
extern u16 zz_8023715C_(void *, u32);
extern u16 zz_80236FFC_(void *, u32);
extern s32 zz_80239984_(s32, void *, u32);
extern void *zz_battle_pokemon_get_stats_start_pointer_c(void *);
extern void zz_80239EE8_(u32, void *, void *, u32, u32, u32, u32, u32);

s32 zz_move_effect_mirror_coat(void *battle, void *defender, u32 messageParam, void *attacker)
{
    u32 targets[8];
    u32 targetCount;
    u16 lastMove;
    u16 i;
    s32 damage;

    damage = 0;
    targetCount = zz_801F1C18_(0, battle, targets, 0, 1);
    lastMove = zz_80236520_(battle, attacker);

    for (i = 0; i < (u16)targetCount; i++) {
        u16 physicalDamage;
        u16 specialDamage;

        physicalDamage = zz_8023715C_(battle, targets[i]);
        specialDamage = zz_80236FFC_(battle, targets[i]);
        if (physicalDamage < specialDamage) {
            damage = zz_80239984_(0, battle, MOVE_ID_356);
            zz_80239EE8_(DAMAGE_MESSAGE, battle, zz_battle_pokemon_get_stats_start_pointer_c(defender), 0, 0, messageParam, 0, MOVE_ID_356);
            break;
        }
    }

    if (lastMove != 0 && lastMove != MOVE_ID_NONE && lastMove != MOVE_ID_357 && lastMove != MOVE_ID_355) {
        if (typeGetCategory(zz_move_get_adjusted_type(battle, lastMove, attacker)) == CATEGORY_SPECIAL) {
            damage = zz_80239984_(damage, battle, MOVE_ID_357);
            zz_80239EE8_(DAMAGE_MESSAGE, battle, zz_battle_pokemon_get_stats_start_pointer_c(defender), 0, 0, messageParam, 0, MOVE_ID_357);
        }
    }
    return damage;
}

s32 zz_move_effect_counter(void *battle, void *defender, u32 messageParam, void *attacker)
{
    u32 targets[8];
    u32 targetCount;
    u16 lastMove;
    u16 i;
    s32 damage;

    damage = 0;
    targetCount = zz_801F1C18_(0, battle, targets, 0, 1);
    lastMove = zz_80236520_(battle, attacker);

    for (i = 0; i < (u16)targetCount; i++) {
        u16 physicalDamage;
        u16 specialDamage;

        physicalDamage = zz_8023715C_(battle, targets[i]);
        specialDamage = zz_80236FFC_(battle, targets[i]);
        if (physicalDamage > specialDamage) {
            damage = zz_80239984_(0, battle, MOVE_ID_354);
            zz_80239EE8_(DAMAGE_MESSAGE, battle, zz_battle_pokemon_get_stats_start_pointer_c(defender), 0, 0, messageParam, 0, MOVE_ID_354);
            break;
        }
    }

    if (lastMove != 0 && lastMove != MOVE_ID_NONE && lastMove != MOVE_ID_357 && lastMove != MOVE_ID_355) {
        if (typeGetCategory(zz_move_get_adjusted_type(battle, lastMove, attacker)) == CATEGORY_PHYSICAL) {
            damage = zz_80239984_(damage, battle, MOVE_ID_355);
            zz_80239EE8_(DAMAGE_MESSAGE, battle, zz_battle_pokemon_get_stats_start_pointer_c(defender), 0, 0, messageParam, 0, MOVE_ID_355);
        }
    }
    return damage;
}
