#ifndef APP_MENU_MENUPOKEMON_H
#define APP_MENU_MENUPOKEMON_H

#include "pokemon/types.h"

u8 menuInputNumber(int initialValue, u32 *value);

u8 menuPokemonSelect(u16 initial, int *selection, char *(*getName)(u16 index));
void menuPokemonSelectClose(void);

extern u32 *pokemonSelectionCountPtr;

#endif
