#ifndef APP_FLOOR_FLOORCOMMAND_H
#define APP_FLOOR_FLOORCOMMAND_H

#include "app/message/message.h"

/*
 * A floor command entry. The menu hands one entry to the callback of each
 * command it shows; the callback fills in the message the entry displays and
 * whether the command can be used.
 */
typedef struct FloorCommandEntry {
    u8 unk_00[6];
    s16 command;
    u8 unk_08[68];
    MessageId messageId; /* 0x4C */
    u8 unk_50[4];       /* 0x50 */
    s16 textX;          /* 0x54 */
} FloorCommandEntry;

void floorCommandSetAvailable(FloorCommandEntry *entry, int available);

#endif
