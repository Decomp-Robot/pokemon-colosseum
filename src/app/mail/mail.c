/*
 * Mail module helpers.
 *
 * mailGetNbMailInMailbox runs the hero party scan from the first slot
 * onwards.  The other three functions answer whether a move may be used
 * against a defender: a move is unusable once its adjusted type is
 * EFF_IMMUNE against the target.
 */
#include "app/mail/mail.h"

#include "app/battle/battleMove.h"
#include "app/hero/heroMove.h"

/* Full use check shared by the two identical entry points. */
static inline s32 isMoveUsable(void *battle, void *attacker, u16 move, void *defender)
{
    s32 usable;
    u16 moveType;

    moveType = zz_move_get_adjusted_type(battle, move, attacker);
    usable = zz_8025C264_(battle, attacker, move, defender, 0);
    if (zz_8023793C_(battle, defender, moveType, zz_80239500_(battle, move)) == EFF_IMMUNE) {
        usable = 0;
    }
    if (usable == 0) {
        return 0;
    } else if (usable == -1) {
        return 1;
    } else {
        return 1;
    }
}

s32 zz_802569B4_(void *battle, void *attacker, u16 move, void *defender)
{
    return isMoveUsable(battle, attacker, move, defender);
}

/* Cheaper check that only looks at the adjusted type. */
s32 zz_80256A68_(void *battle, void *attacker, u16 move, void *defender)
{
    u16 moveType;
    s32 ret;

    moveType = zz_move_get_adjusted_type(battle, move, attacker);
    ret = zz_8023793C_(battle, defender, moveType, zz_80239500_(battle, move)) != EFF_IMMUNE;
    return ret;
}

s32 mailGetNbMailInMailbox(void *data)
{
    u16 slot = 0;

    return -(s32)zz_801F8A18_(data, &slot) != 0;
}

s32 zz_80256B18_(void *battle, void *attacker, u16 move, void *defender)
{
    return isMoveUsable(battle, attacker, move, defender);
}
