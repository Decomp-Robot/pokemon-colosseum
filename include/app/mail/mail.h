#ifndef APP_MAIL_MAIL_H
#define APP_MAIL_MAIL_H

#include "pokemon/types.h"

/*
 * Mail module entry points: the party mail check and the three move-use
 * checks that answer whether a move may be used against a defender.
 */
s32 mailGetNbMailInMailbox(void *data);
s32 zz_802569B4_(void *battle, void *attacker, u16 move, void *defender);
s32 zz_80256A68_(void *battle, void *attacker, u16 move, void *defender);
s32 zz_80256B18_(void *battle, void *attacker, u16 move, void *defender);

#endif
