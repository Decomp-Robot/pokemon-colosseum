#ifndef APP_POKEMON_TRAINER_H
#define APP_POKEMON_TRAINER_H

#include "pokemon/types.h"

typedef struct Trainer Trainer;

typedef enum TrainerValueKind {
    TRAINER_VALUE_UNK_0,
    TRAINER_VALUE_UNK_1,
    TRAINER_VALUE_UNK_2,
    TRAINER_VALUE_PARTY_POKEMON,
    TRAINER_VALUE_UNK_4,
    TRAINER_VALUE_UNK_5,
    TRAINER_VALUE_UNK_6,
    TRAINER_VALUE_UNK_7,
    TRAINER_VALUE_UNK_8,
    TRAINER_VALUE_UNK_9,
    TRAINER_VALUE_UNK_10,
    TRAINER_VALUE_UNK_11,
    TRAINER_VALUE_UNK_12,
    TRAINER_VALUE_UNK_13,
    TRAINER_VALUE_UNK_14,
    TRAINER_VALUE_UNK_15,
    TRAINER_VALUE_UNK_16,
    TRAINER_VALUE_UNK_17,
    TRAINER_VALUE_UNK_18,
    TRAINER_VALUE_UNK_19,
    TRAINER_VALUE_UNK_20,
    TRAINER_VALUE_UNK_21,
    TRAINER_VALUE_UNK_22,
    TRAINER_VALUE_UNK_23,
    TRAINER_VALUE_UNK_24,
    TRAINER_VALUE_UNK_25,
} TrainerValueKind;

#ifdef __cplusplus
extern "C" {
#endif

extern Trainer *selectedTrainer;

u32 getTrainerValueAtIndex(Trainer *trainer, TrainerValueKind kind, int index);

#ifdef __cplusplus
}
#endif

#endif
