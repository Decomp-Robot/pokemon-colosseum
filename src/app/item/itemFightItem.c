/*
 * Battle-item field accessors; a missing item reports zero.
 */
#include "app/item/item.h"

u16 fightItem_GetBuff(Item *item)
{
    if (item == 0) {
        return 0;
    }
    return item->buff;
}

s16 fightItem_GetCount(Item *item)
{
    if (item == 0) {
        return 0;
    }
    return item->count;
}

u8 fightItem_GetTargetDataId(Item *item)
{
    if (item == 0) {
        return 0;
    }
    return item->targetDataId;
}

u16 fightItem_GetItemDataId(Item *item)
{
    if (item == 0) {
        return 0;
    }
    return item->fightItemDataId;
}
