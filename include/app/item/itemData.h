#ifndef APP_ITEM_ITEMDATA_H
#define APP_ITEM_ITEMDATA_H

#include "pokemon/types.h"

typedef struct ItemData ItemData;
typedef int (*ItemUseFunc)(u32 item, u32 *result);

typedef enum ItemGiveIndex {
    ITEM_GIVE_INDEX_INVALID = 255,
} ItemGiveIndex;

typedef enum ItemUseResult {
    ITEM_USE_RESULT_NONE,
    ITEM_USE_RESULT_USED,
    ITEM_USE_RESULT_CANCELLED,
} ItemUseResult;

#ifdef __cplusplus
extern "C" {
#endif

ItemData *getItemDataById(u16 itemId);
u8 getItemGiveIndex(ItemData *data);
int getItemKind(ItemData *data);
ItemUseFunc getBattleUseItemFunc(ItemData *data);
ItemUseFunc getFieldUseItemFunc(ItemData *data);

#ifdef __cplusplus
}
#endif

#endif
