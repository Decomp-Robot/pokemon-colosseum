#ifndef APP_ITEM_ITEM_H
#define APP_ITEM_ITEM_H

#include "pokemon/types.h"

/*
 * Item record shared by the item database and the fight-item accessors.
 */
typedef struct Item {
    u8 unk_000[0x67E];
    u8 targetDataId;
    u8 unk_67F;
    u16 fightItemDataId;
    s16 count;
    u16 buff;
    s16 itemDataId;
    u16 num;
    u8 unk_68A[22];
    u32 unk_6A0;
    s16 numBios;
    u16 friend1Up;
    s16 friend2Up;
    u16 friend3Up;
} Item;

#endif
