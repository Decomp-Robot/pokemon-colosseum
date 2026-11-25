#include "app/pokemon/trainerPokemon.h"

static inline TrainerPokemonSubrecord *getSubrecord(TrainerPokemon *pokemon, u8 index)
{
    if (pokemon == 0) {
        return 0;
    }
    if (index >= TRAINER_POKEMON_SUBRECORD_COUNT) {
        return 0;
    }
    return &pokemon->subrecords[index];
}

s8 trainerPokemonGetNature(TrainerPokemon *pokemon)
{
    if (pokemon == 0) {
        return -1;
    }
    return pokemon->nature;
}

s8 trainerPokemonGetGender(TrainerPokemon *pokemon)
{
    if (pokemon == 0) {
        return -1;
    }
    return pokemon->gender;
}

s16 trainerPokemonGetHappiness(TrainerPokemon *pokemon)
{
    if (pokemon == 0) {
        return -1;
    }
    return pokemon->happiness;
}

u8 trainerPokemonGetSubrecordKind(TrainerPokemon *pokemon, u8 index)
{
    TrainerPokemonSubrecord *subrecord;

    subrecord = getSubrecord(pokemon, index);
    if (subrecord == 0) {
        return 0;
    }
    return subrecord->kind;
}

u32 trainerPokemonGetSubrecordValue(TrainerPokemon *pokemon, u8 index)
{
    TrainerPokemonSubrecord *subrecord;

    subrecord = getSubrecord(pokemon, index);
    if (subrecord == 0) {
        return -1;
    }
    return subrecord->value;
}

u32 trainerPokemonGetItem(TrainerPokemon *pokemon)
{
    if (pokemon == 0) {
        return -1;
    }
    return pokemon->item;
}

u16 trainerPokemonGetSpecies(TrainerPokemon *pokemon)
{
    if (pokemon == 0) {
        return 0;
    }
    return pokemon->species;
}

s8 trainerPokemonGetAbilityIndex(TrainerPokemon *pokemon)
{
    if (pokemon == 0) {
        return -1;
    }
    return pokemon->abilityIndex;
}
