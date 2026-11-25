#ifndef APP_MENU_MENUWINDOW_H
#define APP_MENU_MENUWINDOW_H

#include "pokemon/types.h"

typedef enum MenuWindowId {
    MENU_WINDOW_DEBUG_INPUT = 2,
    MENU_WINDOW_DEBUG_GSGFX = 8,
    MENU_WINDOW_DEBUG_SCRIPT = 10,
    MENU_WINDOW_DEBUG_PEOPLE = 12,
    MENU_WINDOW_PC_STORAGE = 24,
    MENU_WINDOW_PC_PARTY = 25,
    MENU_WINDOW_PC_BOX = 26,
    MENU_WINDOW_PC_LIST = 27,
    MENU_WINDOW_PC_SELECT = 29,
    MENU_WINDOW_PC_SUBTITLE = 30,
    MENU_WINDOW_PC_PROMPT = 31,
    MENU_WINDOW_PC_POKEMON_STORAGE = 32,
    MENU_WINDOW_PC_ITEM_STORAGE = 33,
    MENU_WINDOW_PC_SAVE = 34,
    MENU_WINDOW_PC_CANCEL = 35,
} MenuWindowId;

typedef enum MenuWindowCloseMode {
    MENU_WINDOW_CLOSE_ALL = 0,
} MenuWindowCloseMode;

#ifdef __cplusplus
extern "C" {
#endif

int menuWindowOpen(int windowId, int waitForSelection);
int menuWindowClose(int windowId, int closeMode, int waitUntilClosed);
void menuWindowCloseSingle(int windowId);
u8 menuWindowIsOpen(int windowId);
int menuWindowOpenEx(int windowId, ...);
void menuWindowSetPosition(int windowId, int posX, int posY);

#ifdef __cplusplus
}
#endif

#endif
