#include <libc/string.h>

#include "app/menu/debugMenu.h"
#include "app/pokemon/pokemonEvolution.h"

#define DBG_MENU_EVOLVE_RECORD_SIZE 72

int dbgMenuEvolveItemCreate(DbgMenuItem *item)
{
    if (item->mode == DBG_MENU_ITEM_MODE_CREATE) {
        void *record = dbgMenuAllocateRecord(item, DBG_MENU_EVOLVE_RECORD_SIZE);
        if (record != 0) {
            memcpy(record, item->args[0], DBG_MENU_EVOLVE_RECORD_SIZE);
        }
    }
    dbgMenuGetRecord(item);
    return 0;
}

static inline u8 dbgMenuEvolveItemCheck(void)
{
    if (pokemonEvolutionCheckAvailable(0) != 0) {
        if (pokemonEvolutionCheckSubject(0) == 1 && evolutionGetRefreshStatus() == 1) {
            return 1;
        }
        if (evolutionGetRefreshRequest() == 1) {
            return 1;
        }
    }
    return 0;
}

void dbgMenuEvolveItemUpdate(DbgMenuItem *item)
{
    DbgMenuButton buttons = dbgMenuGetInput()->pressed;
    DbgMenuEvolveResult result = DBG_MENU_EVOLVE_RESULT_NONE;

    if (buttons & DBG_MENU_BUTTON_A) {
        result = DBG_MENU_EVOLVE_RESULT_SELECT;
    } else if (buttons & DBG_MENU_BUTTON_B) {
        result = DBG_MENU_EVOLVE_RESULT_BACK;
    } else if (buttons & DBG_MENU_BUTTON_START) {
        result = DBG_MENU_EVOLVE_RESULT_EXIT;
    } else if (buttons & DBG_MENU_BUTTON_L) {
        item->changed = 1;
        item->refreshPending = 1;
        item->result = DBG_MENU_EVOLVE_RESULT_NONE;
    }

    if (result >= DBG_MENU_EVOLVE_RESULT_SELECT) {
        item->changed = 1;
        item->result = result;
    }

    if (dbgMenuEvolveItemCheck() != 0) {
        item->changed = 1;
        item->refreshPending = 1;
    }
}
