#ifndef POKEMON_ITEM_H
#define POKEMON_ITEM_H

#include "pokemon/types.h"

/*
 * Item record shared by the item database and item-BIOS accessors.  Both
 * accessor families read fields of the same record, so its layout lives here
 * rather than being repeated in each translation unit.  A missing record is
 * treated as one filled with zeroes: every accessor returns zero for null.
 */
typedef struct Item {
    u8 unk_000[1636];
    u8 kind[32];
    u16 buff;
    s16 itemDataId;
    u16 num;
    u8 price;
    u8 coupon;
    u8 doc;
    u8 itemSoubiDataId;
    u8 fightUseKoukaDataId;
    u8 itemBuff;
    u8 itemEffectParam;
    u8 fieldUseFunc;
    u8 battleUseFunc;
    u8 fightKoukaDataId;
    u8 inWzxDataId;
    u8 openWzxDataId;
    u8 outWzxDataId;
    u8 downinWzxDataId;
    u8 throwWzxDataId;
    u8 snatchAttackWzxDataId;
    u8 snatchBalllandWzxDataId;
    u8 snatchMissWzxDataId;
    u8 snatchPokeoutWzxDataId;
    u8 snatchShakeWzxDataId;
    u8 snatchSnatchWzxDataId;
    u8 unk_69F;
    u32 itemDataIdBios;
    s16 numBios;
    u16 friend1Up;
    s16 friend2Up;
    u16 friend3Up;
} Item;

/* Item database accessors (itemDB.c). */
u16 item_GetNum(Item *item);
s16 item_GetItemDataId(Item *item);

/* Item BIOS field accessors (itemBios.c). */
u16 itemParamGetFriend3Up(const Item *bios);
s16 itemParamGetFriend2Up(const Item *bios);
u16 itemParamGetFriend1Up(const Item *bios);
s16 itemBiosGetNum(const Item *bios);
u32 itemBiosGetItemDataId(const Item *bios);
u8 itemBallDataBiosGetSnatchSnatchWzxDataId(const Item *bios);
u8 itemBallDataBiosGetSnatchShakeWzxDataId(const Item *bios);
u8 itemBallDataBiosGetSnatchPokeoutWzxDataId(const Item *bios);
u8 itemBallDataBiosGetSnatchMissWzxDataId(const Item *bios);
u8 itemBallDataBiosGetSnatchBalllandWzxDataId(const Item *bios);
u8 itemBallDataBiosGetSnatchAttackWzxDataId(const Item *bios);
u8 itemBallDataBiosGetThrowWzxDataId(const Item *bios);
u8 itemBallDataBiosGetDowninWzxDataId(const Item *bios);
u8 itemBallDataBiosGetOutWzxDataId(const Item *bios);
u8 itemBallDataBiosGetOpenWzxDataId(const Item *bios);
u8 itemBallDataBiosGetInWzxDataId(const Item *bios);
u8 itemBallDataBiosGetFightKoukaDataId(const Item *bios);
u8 itemDataBiosGetBattleUseFunc(const Item *bios);
u8 itemDataBiosGetFieldUseFunc(const Item *bios);
u8 itemDataBiosGetItemEffectParam(const Item *bios);
u8 itemDataBiosGetBuff(const Item *bios);
u8 itemDataBiosGetFightUseKoukaDataId(const Item *bios);
u8 itemDataBiosGetItemSoubiDataId(const Item *bios);
u8 itemDataBiosGetDoc(const Item *bios);
u8 itemDataBiosGetCoupon(const Item *bios);
u8 itemDataBiosGetPrice(const Item *bios);
const u8 *itemDataBiosGetKind(const Item *bios);

#endif
