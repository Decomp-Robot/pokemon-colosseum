#include "app/floor/floorCommand.h"
#include "app/menu/debugMenu.h"
#include "app/menu/menuWindow.h"
#include "app/menu/screenElement.h"
#include "app/message/message.h"
#include "pokemon/types.h"

void dbgMenuLogShow(MenuWindow *window, FloorCommandEntry *entry)
{
    DbgMenuItem *subject;
    const DbgMenuLog *logData;
    s32 slot;
    s32 color;

    slot = -1;

    subject = findDebugMenuSubject(menuValueGet(window, MENU_VALUE_DBG_MENU_LOG_SUBJECT_ID));
    if (subject == 0) {
        return;
    }
    logData = dbgMenuGetRecord(subject);
    if (logData == 0) {
        return;
    }

    color = getMenuEntryColor(window, entry);

    switch (entry->command) {
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_0:
        slot = 0;
        break;
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_1:
        slot = 1;
        break;
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_2:
        slot = 2;
        break;
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_3:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_3:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_3:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_3:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_3:
        slot = 3;
        break;
    }

    if (slot < 0) {
        return;
    }

    if (logData->moves[slot].moveName) {
        floorCommandSetAvailable(entry, 1);
    } else {
        floorCommandSetAvailable(entry, 0);
        return;
    }

    switch (entry->command) {
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ROW_SLOT_3:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_ICON_SLOT_3:
        break;
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_TYPE_ICON_SLOT_3:
        showMenuMessage(0, 0, window, logData->moves[slot].typeIconId, 0);
        break;
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_PP_SLOT_3: {
        s16 slashWidth = measureMessageText(MESSAGE_ID_SLASH) >> 16;
        s16 slashX;

        drawMessageWindow(slashX = (entry->textX - slashWidth) / 2, 0, color, MESSAGE_ID_SLASH);
        setMessageVariable(MESSAGE_VARIABLE_NUMBER, logData->moves[slot].currentPP);
        drawMessageWithVariables(0, 0, slashX, entry->textY, color, MESSAGE_ID_NUMBER_TEMPLATE);
        setMessageVariable(MESSAGE_VARIABLE_NUMBER, logData->moves[slot].maxPP);
        drawMessageWithVariables(0, 0, entry->textX, entry->textY, color, MESSAGE_ID_NUMBER_TEMPLATE);
        break;
    }
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_0:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_1:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_2:
    case SCREEN_ELEMENT_DBG_MENU_LOG_MOVE_NAME_SLOT_3:
        setMessageVariable(MESSAGE_VARIABLE_STRING, (u32)logData->moves[slot].moveName);
        drawMessageWindow(0, 0, color, MESSAGE_ID_STRING_TEMPLATE);
        break;
    }
}
