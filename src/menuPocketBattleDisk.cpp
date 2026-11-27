#include "app/menu/menuWindow.h"
#include "app/menu/pocket.h"
#include "app/message/message.h"
#include "app/pokemon/battleDisk.h"
#include "pokemon/types.h"

extern "C" {

static inline u16 findSelectedItem(int pocket, int row, u16 *count)
{
    PocketItem *item;
    int i;
    int index;
    int kind = pocketDescriptors[pocket].pocketKind;

    if (kind >= 0) {
        item = getTrainerPocket(selectedTrainer, (u8)kind, count, 0, 0, 0);
    } else {
        item = getDefaultPocket(selectedTrainer, count, 0, 0, 0);
    }

    index = -1;
    for (i = 0; i < *count; i++, item++) {
        if (isPocketItemValid(item)) {
            index++;
            if (index >= row) {
                return getPocketItemId(item);
            }
        }
    }

    return 0;
}

int givePocketItem(int pocket, int row, u32 *result)
{
    u16 count;
    u16 itemId;
    u8 kind;

    itemId = findSelectedItem(pocket, row, &count);
    if (getItemGiveIndex(getItemDataById(itemId)) != ITEM_GIVE_INDEX_INVALID) {
        setMessageVariable(MESSAGE_VARIABLE_ITEM_NAME, itemId);
        displayMessage(MESSAGE_DISPLAY_KIND_MENU, MESSAGE_ID_ITEM_CANNOT_GIVE,
                       MESSAGE_DISPLAY_ARGUMENT_SET, MESSAGE_DISPLAY_ARGUMENT_NONE);
        closeMessageWindow(MESSAGE_WINDOW_CLOSE_WAIT);
        *result = 0;
        return POCKET_MENU_RESULT_CONTINUE;
    }

    kind = (u8)getItemKind(getItemDataById((u16)itemId));
    menuWindowClose(MENU_WINDOW_POCKET_ITEM, MENU_WINDOW_CLOSE_ALL, MENU_WINDOW_WAIT_FOR_CLOSE);
    pocketItemResult = menuPokemonOpenItemGive(kind, (u8)row, itemId, 0);
    closeItemMenu();
    if ((s32)pocketItemResult < 0) {
        return POCKET_MENU_RESULT_CLOSE;
    }

    *result = 0;
    return POCKET_MENU_RESULT_CONTINUE;
}

int usePocketItem(int pocket, int row, u32 *result)
{
    u16 count;
    int itemId;
    ItemData *data;
    ItemUseFunc useFunc;
    int used;
    int ret;

    itemId = findSelectedItem(pocket, row, &count);
    data = getItemDataById((u16)itemId);
    if ((s32)selectedFieldIndex == POCKET_FIELD_MODE_FIELD) {
        useFunc = getBattleUseItemFunc(data);
    } else {
        useFunc = getFieldUseItemFunc(data);
    }

    if (useFunc == 0) {
        displayMessage(MESSAGE_DISPLAY_KIND_MENU, MESSAGE_ID_ITEM_CANNOT_USE,
                       MESSAGE_DISPLAY_ARGUMENT_SET, MESSAGE_DISPLAY_ARGUMENT_NONE);
        closeMessageWindow(MESSAGE_WINDOW_CLOSE_WAIT);
        *result = 0;
        return POCKET_MENU_RESULT_CLOSE;
    }

    used = useFunc((u32)itemId, result);
    switch (used) {
    case ITEM_USE_RESULT_NONE:
        ret = POCKET_MENU_RESULT_CONTINUE;
        break;
    case ITEM_USE_RESULT_USED:
        ret = POCKET_MENU_RESULT_CLOSE;
        break;
    case ITEM_USE_RESULT_CANCELLED:
        ret = POCKET_MENU_RESULT_REFRESH;
        break;
    }
    return ret;
}

void getPocketItemContext(Trainer **trainerOut, BattleDiskFieldData **fieldOut)
{
    *trainerOut = selectedTrainer;
    *fieldOut = fieldPokemonData;
}

u32 getPocketItemMenuResult(void)
{
    return pocketItemResult;
}

void getPocketItemValue(u16 index, u32 *valueOut, BattleDiskPokemon **slotOut)
{
    BattleDiskPokemon *slot;
    u32 value;

    if ((s32)selectedFieldIndex == POCKET_FIELD_MODE_BATTLE_DISK) {
        slot = getBattleDiskPokemon(fieldPokemonData, (u16)index);
        value = getBattleDiskPokemonItem(slot);
    } else {
        value = getTrainerValueAtIndex(selectedTrainer, TRAINER_VALUE_PARTY_POKEMON, (int)(u16)index);
        slot = 0;
    }

    *valueOut = value;
    *slotOut = slot;
}

}

namespace {

__declspec(weak) void getItemNameMsg(u32 messageId)
{
    pocketItemResult = messageId;
    closeItemMenu();
}

}
