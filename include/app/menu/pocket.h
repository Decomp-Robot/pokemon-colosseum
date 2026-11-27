#ifndef APP_MENU_POCKET_H
#define APP_MENU_POCKET_H

#include "app/item/itemData.h"
#include "app/pokemon/battleDisk.h"
#include "app/pokemon/trainer.h"
#include "pokemon/types.h"

typedef struct PocketItem {
    u16 itemId;
    u8 rest[2];
} PocketItem;

typedef struct PocketDescriptor {
    u32 rest;
    s32 pocketKind;
    u8 trailing[68];
} PocketDescriptor;

typedef enum PocketFieldMode {
    POCKET_FIELD_MODE_FIELD,
    POCKET_FIELD_MODE_BATTLE_DISK,
} PocketFieldMode;

typedef enum PocketMenuResult {
    POCKET_MENU_RESULT_CONTINUE,
    POCKET_MENU_RESULT_CLOSE = 3,
    POCKET_MENU_RESULT_REFRESH = 4,
} PocketMenuResult;

#ifdef __cplusplus
extern "C" {
#endif

extern u32 selectedFieldIndex;
extern u32 pocketItemResult;
extern PocketDescriptor pocketDescriptors[];

PocketItem *getTrainerPocket(Trainer *trainer, u8 kind, u16 *count, u16 *stackLimit, u8 *flagA, u8 *flagB);
PocketItem *getDefaultPocket(Trainer *trainer, u16 *count, u16 *stackLimit, u8 *flagA, u8 *flagB);
u8 isPocketItemValid(PocketItem *item);
u16 getPocketItemId(PocketItem *item);

int menuPokemonOpenItemGive(u8 item, u8 from, u16 count, u32 data);
int closeItemMenu(void);

#ifdef __cplusplus
}
#endif

#endif
