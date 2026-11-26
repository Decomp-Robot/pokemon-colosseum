#ifndef APP_POKEMON_POKEMONEVOLUTION_H
#define APP_POKEMON_POKEMONEVOLUTION_H

#include "pokemon/types.h"

#ifdef __cplusplus
extern "C" {
#endif

u8 pokemonEvolutionCheckAvailable(void *context);
u8 pokemonEvolutionCheckSubject(void *context);
u8 evolutionGetRefreshStatus(void);
u16 evolutionGetRefreshRequest(void);

#ifdef __cplusplus
}
#endif

#endif
