#ifndef APP_BATTLE_BATTLEMOVE_H
#define APP_BATTLE_BATTLEMOVE_H

#include "pokemon/types.h"

/*
 * Move values the battle modules share.  The adjusted type is the type a
 * move really has once battle effects have changed it; the match-up of that
 * type against the defender is reported as a TypeEffectiveness code.
 */
u16 zz_move_get_adjusted_type(void *battle, u16 move, void *pokemon);
s16 zz_80239500_(void *battle, u16 move);
u16 zz_8023793C_(void *battle, void *defender, u16 moveType, s16 power);

/*
 * Checks whether the battle allows the attacker to use the move against the
 * defender.  It reports -1, 0 or 1, and the callers treat 0 as forbidden and
 * both other results as usable.
 */
s32 zz_8025C264_(void *battle, void *attacker, u16 move, void *defender, s32 flags);

#endif
