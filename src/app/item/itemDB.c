/*
 * Item database accessors; a missing item reports zero.
 */
#include "pokemon/types.h"

typedef struct Item {
    u8 unk_000[0x684];
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

u16 item_GetNum(Item *item)
{
    if (item == 0) {
        return 0;
    }
    return item->num;
}

s16 item_GetItemDataId(Item *item)
{
    if (item == 0) {
        return 0;
    }
    return item->itemDataId;
}
