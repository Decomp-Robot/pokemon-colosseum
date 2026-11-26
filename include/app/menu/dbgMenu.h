#ifndef APP_MENU_DBGMENU_H
#define APP_MENU_DBGMENU_H

#include "pokemon/types.h"

enum DbgMenuCallbackResult {
    DBG_MENU_CALLBACK_DISABLED = -1,
    DBG_MENU_CALLBACK_COMPLETE = 1
};

u8 dbgTriggerIsEnabled(void);
char *dbgMenuFightGetPokemonName(u16 index);
int dbgMenuFightEditSelectedPokemon(u16 index);
int dbgMenuFightFightPokemonSelectCallback(void);

#endif
