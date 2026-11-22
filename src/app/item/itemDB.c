/*
 * Item database accessors; a missing item reports zero.
 */
#include "app/item/item.h"

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
