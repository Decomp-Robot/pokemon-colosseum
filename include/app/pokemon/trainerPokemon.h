#ifndef APP_POKEMON_TRAINERPOKEMON_H
#define APP_POKEMON_TRAINERPOKEMON_H

#include "pokemon/types.h"

#define TRAINER_POKEMON_SUBRECORD_COUNT 4

/*
 * A trainer's Pokemon record.  Trainers carry a table of these, indexed by
 * the Pokemon's id, so the record has a fixed stride of 0x50 bytes.
 */
typedef struct TrainerPokemonSubrecord {
    u8 kind;
    u8 pad[3];
    u32 value;
} TrainerPokemonSubrecord;

typedef struct TrainerPokemon {
    u8 abilityIndex;       /* 0x00 */
    u8 gender;             /* 0x01 */
    u8 nature;             /* 0x02 */
    u8 unk_03;             /* 0x03 */
    u8 unk_04;             /* 0x04 */
    u8 unk_05[3];          /* 0x05 */
    s16 happiness;         /* 0x08 */
    u16 species;           /* 0x0A */
    u16 unk_0C;            /* 0x0C */
    u16 unk_0E;            /* 0x0E */
    u32 item;              /* 0x10 */
    u32 unk_14;            /* 0x14 */
    u8 unk_18[4];          /* 0x18 */
    u8 unk_1C[6];          /* 0x1C */
    s16 unk_22[6];         /* 0x22 */
    u8 unk_2E[2];          /* 0x2E */
    TrainerPokemonSubrecord subrecords[TRAINER_POKEMON_SUBRECORD_COUNT];
} TrainerPokemon;

s8 trainerPokemonGetNature(TrainerPokemon *pokemon);
s8 trainerPokemonGetGender(TrainerPokemon *pokemon);
s16 trainerPokemonGetHappiness(TrainerPokemon *pokemon);
u8 trainerPokemonGetSubrecordKind(TrainerPokemon *pokemon, u8 index);
u32 trainerPokemonGetSubrecordValue(TrainerPokemon *pokemon, u8 index);
u32 trainerPokemonGetItem(TrainerPokemon *pokemon);
u16 trainerPokemonGetSpecies(TrainerPokemon *pokemon);
s8 trainerPokemonGetAbilityIndex(TrainerPokemon *pokemon);

#endif
