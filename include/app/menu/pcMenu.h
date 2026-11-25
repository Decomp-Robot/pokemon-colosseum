#ifndef APP_MENU_PCMENU_H
#define APP_MENU_PCMENU_H

#include "app/menu/menuModel.h"
#include "app/message/message.h"
#include "pokemon/types.h"

typedef enum PcMenuOption {
    PC_MENU_OPTION_POKEMON_STORAGE,
    PC_MENU_OPTION_ITEM_STORAGE,
    PC_MENU_OPTION_SAVE,
    PC_MENU_OPTION_CANCEL,
    PC_MENU_OPTION_COUNT,
} PcMenuOption;

typedef struct PcMenuModel {
    MenuModel model;
    u32 savedReturnMap;
    u32 savedReturnSpawn;
    u8 savedReturnFlag;
} PcMenuModel;

typedef enum PcMenuState {
    PC_MENU_STATE_START,
    PC_MENU_STATE_SELECT,
    PC_MENU_STATE_POKEMON_STORAGE = 10,
    PC_MENU_STATE_ITEM_STORAGE = 20,
    PC_MENU_STATE_SAVE = 30,
    PC_MENU_STATE_LEAVE = 40,
    PC_MENU_STATE_END = 1000,
} PcMenuState;

#ifdef __cplusplus
extern "C" {
#endif

extern PcMenuModel pcMenuOptionModel;
extern MessageId pcMenuPrompt;
extern s8 pcMenuRequestedOption;
extern s8 pcMenuShownOption;
extern float pcMenuCurrentAngle;
extern float pcMenuTargetAngle;
extern s8 pcMenuSelectedOption;

void pcMenuLoop(void);

PcMenuState confirmAndSaveGame(void);

#ifdef __cplusplus
}
#endif

#endif
