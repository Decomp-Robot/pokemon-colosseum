#ifndef APP_POKEMON_POKEMON_H
#define APP_POKEMON_POKEMON_H

#include "pokemon/types.h"

/* Gender of a Pokemon record, as reported by its gender accessor. */
typedef enum PokemonGender {
    POKEMON_GENDER_MALE,
    POKEMON_GENDER_FEMALE,
    POKEMON_GENDER_GENDERLESS,
    POKEMON_GENDER_UNKNOWN = 255,
} PokemonGender;

/*
 * A Pokemon record is 312 bytes. The record holds everything the game knows
 * about one Pokemon; its fields are read through the accessors of the
 * Pokemon modules.
 */
typedef struct Pokemon {
    u8 data[312];
} Pokemon;

#endif
