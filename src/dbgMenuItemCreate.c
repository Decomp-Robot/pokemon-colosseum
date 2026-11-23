/*
 * Item creation commands of the debug menu.
 *
 * One of them copies the data block of an entry into the buffer the menu
 * allocates for it, the other one lets the pad choose the value the entry
 * reports.
 */
#include "pokemon/types.h"
#include "menuEntry.h"
#include "padState.h"
#include <dolphin/pad.h>
#include <libc/string.h>

/* Bytes copied out of the data block of an entry. */
#define DATA_BLOCK_SIZE 72

extern PadState *zz_80105624_(void);
extern void *zz_80103FFC_(MenuEntry *entry, int size);
extern void *zz_80103FE4_(MenuEntry *entry);
extern u8 zz_801F18DC_(int index);
extern u8 zz_801F1700_(int index);
extern u8 zz_80265924_(void);
extern u16 zz_801EF634_(void);

/*
 * Says whether the game state leaves the entry without a value, whichever
 * button the pad reports.
 */
static inline u8 dbgMenuValueBlocked(void)
{
    if (zz_801F18DC_(0)) {
        if (zz_801F1700_(0) == 1 && zz_80265924_() == 1) {
            return 1;
        }
        if (zz_801EF634_() == 1) {
            return 1;
        }
    }
    return 0;
}

int zz_8000FDD8_(MenuEntry *entry)
{
    void *record;

    if (entry->state == 0) {
        record = zz_80103FFC_(entry, DATA_BLOCK_SIZE);
        if (record != 0) {
            memcpy(record, entry->dataBlock, DATA_BLOCK_SIZE);
        }
    }
    zz_80103FE4_(entry);
    return 0;
}

void zz_8000FE38_(MenuEntry *entry)
{
    u16 buttons;
    MenuValue value;

    buttons = zz_80105624_()->pressed;
    value = MENU_VALUE_NONE;
    if (buttons & PAD_TRIGGER_Z) {
        value = MENU_VALUE_FIRST;
    } else if (buttons & PAD_TRIGGER_R) {
        value = MENU_VALUE_THIRD;
    } else if (buttons & PAD_BUTTON_Y) {
        value = MENU_VALUE_FOURTH;
    } else if (buttons & PAD_BUTTON_B) {
        entry->finished = 1;
        entry->noValue = 1;
        entry->value = MENU_VALUE_NONE;
    }
    if (value >= 0) {
        entry->finished = 1;
        entry->value = value;
    }
    if (dbgMenuValueBlocked()) {
        entry->finished = 1;
        entry->noValue = 1;
    }
}
