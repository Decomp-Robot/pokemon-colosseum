#ifndef APP_POKEMON_BATTLEDISK_H
#define APP_POKEMON_BATTLEDISK_H

#include "pokemon/types.h"

typedef struct BattleDiskFieldData BattleDiskFieldData;
typedef struct BattleDiskPokemon BattleDiskPokemon;

#ifdef __cplusplus
extern "C" {
#endif

extern BattleDiskFieldData *fieldPokemonData;

BattleDiskPokemon *getBattleDiskPokemon(BattleDiskFieldData *fieldData, u16 index);
u32 getBattleDiskPokemonItem(BattleDiskPokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
