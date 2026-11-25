#ifndef APP_MESSAGE_MESSAGE_H
#define APP_MESSAGE_MESSAGE_H

#include "pokemon/types.h"

/*
 * Ids of the messages in the shared archive, grouped by the screen that
 * shows them. Id 0 selects no message.
 */
typedef enum MessageId {
    MESSAGE_ID_NONE = 0,
    MESSAGE_ID_FLOOR_READ_RESULT = 222,
    MESSAGE_ID_FLOOR_READ_SPECIES = 231,
    MESSAGE_ID_FLOOR_READ_GENDER_MALE = 3431,
    MESSAGE_ID_FLOOR_READ_GENDER_FEMALE = 3432,
    MESSAGE_ID_PC_WHICH_TO_ACCESS = 7002,
    MESSAGE_ID_PC_WHAT_TO_DO = 7009,
    MESSAGE_ID_FLOOR_READ_LEVEL = 7042,
} MessageId;

/* Slots of the message variable block that message text substitutes. */
typedef enum MessageVariable {
    MESSAGE_VARIABLE_FLOOR_READ_LEVEL = 52,
    MESSAGE_VARIABLE_FLOOR_READ_SPECIES_NAME = 55,
} MessageVariable;

/* Stores a value in one slot of the message variable block. */
void zz_set_msg_var(int variable, u32 value);

#endif
