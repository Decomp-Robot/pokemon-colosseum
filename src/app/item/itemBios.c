/*
 * Field accessors for an item's BIOS data record.
 *
 * A missing record is treated as one filled with zeroes: the getters return
 * zero.  The record holds the counts and bonus values shared by every item,
 * the ball effect numbers, and the item-data table parameters.
 */
#include "pokemon/item.h"

u16 itemParamGetFriend3Up(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->friend3Up;
}

s16 itemParamGetFriend2Up(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->friend2Up;
}

u16 itemParamGetFriend1Up(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->friend1Up;
}

s16 itemBiosGetNum(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->numBios;
}

u32 itemBiosGetItemDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->itemDataIdBios;
}

u8 itemBallDataBiosGetSnatchSnatchWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->snatchSnatchWzxDataId;
}

u8 itemBallDataBiosGetSnatchShakeWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->snatchShakeWzxDataId;
}

u8 itemBallDataBiosGetSnatchPokeoutWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->snatchPokeoutWzxDataId;
}

u8 itemBallDataBiosGetSnatchMissWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->snatchMissWzxDataId;
}

u8 itemBallDataBiosGetSnatchBalllandWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->snatchBalllandWzxDataId;
}

u8 itemBallDataBiosGetSnatchAttackWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->snatchAttackWzxDataId;
}

u8 itemBallDataBiosGetThrowWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->throwWzxDataId;
}

u8 itemBallDataBiosGetDowninWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->downinWzxDataId;
}

u8 itemBallDataBiosGetOutWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->outWzxDataId;
}

u8 itemBallDataBiosGetOpenWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->openWzxDataId;
}

u8 itemBallDataBiosGetInWzxDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->inWzxDataId;
}

u8 itemBallDataBiosGetFightKoukaDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->fightKoukaDataId;
}

u8 itemDataBiosGetBattleUseFunc(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->battleUseFunc;
}

u8 itemDataBiosGetFieldUseFunc(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->fieldUseFunc;
}

u8 itemDataBiosGetItemEffectParam(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->itemEffectParam;
}

u8 itemDataBiosGetBuff(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->itemBuff;
}

u8 itemDataBiosGetFightUseKoukaDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->fightUseKoukaDataId;
}

u8 itemDataBiosGetItemSoubiDataId(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->itemSoubiDataId;
}

u8 itemDataBiosGetDoc(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->doc;
}

u8 itemDataBiosGetCoupon(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->coupon;
}

u8 itemDataBiosGetPrice(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->price;
}

const u8 *itemDataBiosGetKind(const Item *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->kind;
}
