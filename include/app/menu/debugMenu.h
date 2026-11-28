#ifndef APP_MENU_DEBUGMENU_H
#define APP_MENU_DEBUGMENU_H

#include "pokemon/types.h"

struct BattlePokemon;
struct FloorCommandEntry;
struct MenuWindow;

typedef struct DbgMenuLogEntry {
    char *moveName;
    char *typeName;
    u16 typeIconId;
    u8 maxPP;
    u8 currentPP;
} DbgMenuLogEntry;

typedef struct DbgMenuLog {
    char *pokemonName;
    DbgMenuLogEntry moves[4];
    u8 unk_34[12];
    struct BattlePokemon *battler;
    u8 canEvolve;
} DbgMenuLog;

typedef enum DbgMenuItemMode {
    DBG_MENU_ITEM_MODE_CREATE,
    DBG_MENU_ITEM_MODE_PENDING,
    DBG_MENU_ITEM_MODE_ACTIVE,
    DBG_MENU_ITEM_MODE_SUBMENU,
    DBG_MENU_ITEM_MODE_DELEGATE,
    DBG_MENU_ITEM_MODE_FINALIZE,
} DbgMenuItemMode;

typedef enum DbgMenuEvolveResult {
    DBG_MENU_EVOLVE_RESULT_NONE = -1,
    DBG_MENU_EVOLVE_RESULT_SELECT = 0,
    DBG_MENU_EVOLVE_RESULT_BACK = 2,
    DBG_MENU_EVOLVE_RESULT_EXIT = 3,
} DbgMenuEvolveResult;

typedef enum DbgMenuButton {
    DBG_MENU_BUTTON_UP = 1 << 0,
    DBG_MENU_BUTTON_DOWN = 1 << 1,
    DBG_MENU_BUTTON_LEFT = 1 << 2,
    DBG_MENU_BUTTON_RIGHT = 1 << 3,
    DBG_MENU_BUTTON_A = 1 << 4,
    DBG_MENU_BUTTON_B = 1 << 5,
    DBG_MENU_BUTTON_X = 1 << 6,
    DBG_MENU_BUTTON_Y = 1 << 7,
    DBG_MENU_BUTTON_Z = 1 << 8,
    DBG_MENU_BUTTON_L = 1 << 9,
    DBG_MENU_BUTTON_R = 1 << 10,
    DBG_MENU_BUTTON_START = 1 << 11,
} DbgMenuButton;

typedef struct DbgMenuInput {
    u8 reserved0[4];
    u16 pressed;
    u8 reserved1[20];
} DbgMenuInput;

typedef struct DbgMenuItem {
    u8 reserved0;
    s8 mode;
    u8 reserved1[2];
    u32 itemId;
    u8 reserved2[88];
    void *args[8];
    DbgMenuEvolveResult result;
    u8 reserved3[20];
    u8 changed;
    u8 refreshPending;
    u8 reserved4[18];
    u16 recordHandle;
    void *record;
} DbgMenuItem;

#ifdef __cplusplus
extern "C" {
#endif

void *dbgMenuAllocateRecord(DbgMenuItem *item, int size);
void *dbgMenuGetRecord(DbgMenuItem *item);
const DbgMenuInput *dbgMenuGetInput(void);

int dbgMenuEvolveItemCreate(DbgMenuItem *item);
void dbgMenuEvolveItemUpdate(DbgMenuItem *item);
int dbgMenuReliveItemCreate(DbgMenuItem *item);

void dbgMenuLogShow(struct MenuWindow *window, struct FloorCommandEntry *entry);

DbgMenuItem *findDebugMenuSubject(s32 subjectId);

#ifdef __cplusplus
}
#endif

#endif
