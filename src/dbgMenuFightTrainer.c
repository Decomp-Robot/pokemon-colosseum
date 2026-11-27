#include "app/floor/floorCommand.h"
#include "app/menu/dbgMenuFightTrainer.h"
#include "app/menu/menuWindow.h"
#include "app/menu/screenElement.h"

void dbgMenuFightTrainerSetAvailable(MenuWindow *window, FloorCommandEntry *entry)
{
    int available = 1;

    if (window->id != MENU_WINDOW_FIGHT_TRAINER_DATA &&
        window->id == MENU_WINDOW_FIGHT_TRAINER_SELECT &&
        (u8)menuValueGet(window, MENU_VALUE_FIGHT_TRAINER_DATA_AVAILABLE) == 0) {
        available = 0;
    }

    switch (entry->command) {
    case SCREEN_ELEMENT_FIGHT_TRAINER_COMMAND_SELECT:
    case SCREEN_ELEMENT_FIGHT_TRAINER_COMMAND_AI_DATA:
    case SCREEN_ELEMENT_FIGHT_TRAINER_COMMAND_CANCEL:
        floorCommandSetAvailable(entry, 1);
        break;
    case SCREEN_ELEMENT_FIGHT_TRAINER_COMMAND_DATA:
    case SCREEN_ELEMENT_FIGHT_TRAINER_COMMAND_AI_DATA_EDIT:
    case SCREEN_ELEMENT_FIGHT_TRAINER_COMMAND_START:
        floorCommandSetAvailable(entry, available);
        break;
    }
}
