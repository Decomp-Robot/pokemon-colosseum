#ifndef MENU_ENTRY_H
#define MENU_ENTRY_H

#include "pokemon/types.h"

/*
 * Value an entry reports for itself: one of the four answers its command can
 * pick, or MENU_VALUE_NONE when it picked none.
 */
typedef enum MenuValue {
    MENU_VALUE_NONE = -1,
    MENU_VALUE_FIRST,
    MENU_VALUE_SECOND,
    MENU_VALUE_THIRD,
    MENU_VALUE_FOURTH
} MenuValue;

/*
 * One slot of the menu's entry table. The menu code creates an entry in the
 * first free slot of the table, links it into the list ordered by priority and
 * hands it to the entry's command, which fills in the value the entry reports.
 */
typedef struct MenuEntry {
    u8 unk_00;
    s8 state;
    u8 unk_02[2];
    int id;
    u8 unk_08;
    s8 priority;
    u8 unk_0A[2];
    struct MenuEntry *parent;
    struct MenuEntry *next;
    struct MenuEntry *prev;
    u8 unk_18[72];
    void *dataBlock;
    u8 unk_64[28];
    MenuValue value;
    u8 unk_84[20];
    u8 finished; /* the entry has nothing left to run */
    u8 noValue;  /* the entry reports no value */
    u8 unk_9A[18];
    u16 workHandle;
    u16 unk_AE;
    void *workBuffer;
} MenuEntry;

#endif
