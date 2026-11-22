/*
 * Accessors for the evolution table stored in a species' stats block.
 *
 * A species has up to five evolution records. Each record describes the
 * method that triggers the evolution, the condition that must be met and
 * the species evolved into. Missing stats or an out-of-range record reads
 * as zero.
 */
#include "pokemon/types.h"

#define EVOLUTION_COUNT 5

typedef struct EvolutionRecord {
    u8 method;
    u8 unused;
    u16 condition;
    u16 species;
} EvolutionRecord;

typedef struct PokemonStats {
    u8 unk_000[156];
    EvolutionRecord evolution[EVOLUTION_COUNT];
} PokemonStats;

static inline const EvolutionRecord *getEvolution(const PokemonStats *stats, u16 index)
{
    if (stats == 0) {
        return 0;
    }
    if (index >= EVOLUTION_COUNT) {
        return 0;
    }
    return &stats->evolution[index];
}

u16 pokemonStats_GetSpeciesEvolutionWithIndex(const PokemonStats *stats, u16 index)
{
    const EvolutionRecord *evolution;

    evolution = getEvolution(stats, index);
    if (evolution == 0) {
        return 0;
    }
    return evolution->species;
}

u16 pokemonStats_GetConditionEvolutionWithIndex(const PokemonStats *stats, u16 index)
{
    const EvolutionRecord *evolution;

    evolution = getEvolution(stats, index);
    if (evolution == 0) {
        return 0;
    }
    return evolution->condition;
}

u8 pokemonStats_GetMethodEvolutionWithIndex(const PokemonStats *stats, u16 index)
{
    const EvolutionRecord *evolution;

    evolution = getEvolution(stats, index);
    if (evolution == 0) {
        return 0;
    }
    return evolution->method;
}
