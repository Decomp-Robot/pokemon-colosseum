/*
 * Field accessors for the trainer-Pokemon record. A missing Pokemon reports
 * -1 for each of these fields.
 */
#include "app/pokemon/trainerPokemon.h"

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
