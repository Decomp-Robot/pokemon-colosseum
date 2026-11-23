#ifndef APP_POKEMON_POKEMONSTATS_H
#define APP_POKEMON_POKEMONSTATS_H

#include "pokemon/types.h"

/*
 * A species' stats block. Only the evolution records that the accessors below
 * read are declared; the rest of the block is private to the stats module.
 */
typedef struct PokemonStats PokemonStats;

u16 pokemonStats_GetSpeciesEvolutionWithIndex(const PokemonStats *stats, u16 index);
u16 pokemonStats_GetConditionEvolutionWithIndex(const PokemonStats *stats, u16 index);
u8 pokemonStats_GetMethodEvolutionWithIndex(const PokemonStats *stats, u16 index);

#endif
