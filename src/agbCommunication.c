/*
 * Availability callbacks for the AGB communication menu.
 *
 * Each entry of the menu asks one of these functions whether its option can
 * be used right now. The shared guard supplies the subject being checked;
 * without one every option stays disabled. Otherwise a byte in that subject
 * selects the offered activities, and the resulting yes/no answer is written
 * back to the menu entry.
 */
#include "pokemon/types.h"

/* Bits of the communication state byte tested by the individual entries. */
enum AgbCommunicationStateFlag {
    AGB_STATE_FLAG_0 = 0x01,
    AGB_STATE_FLAG_1 = 0x02,
    AGB_STATE_FLAG_2 = 0x04,
    AGB_STATE_FLAG_3 = 0x08,
};

extern void *zz_80057270_(void);
extern u8 zz_8011E820_(void *state);
extern void zz_80109220_(void *entry, int available);

int zz_80053A60_(void *menu, void *entry)
{
    u8 available = 0;
    void *state = zz_80057270_();

    if (state != 0) {
        if ((zz_8011E820_(state) & AGB_STATE_FLAG_3) != 0) {
            available = 1;
        }
    }
    zz_80109220_(entry, available);
    return 0;
}

int zz_80053AC8_(void *menu, void *entry)
{
    u8 available = 0;
    void *state = zz_80057270_();

    if (state != 0) {
        if ((zz_8011E820_(state) & AGB_STATE_FLAG_2) != 0) {
            available = 1;
        }
    }
    zz_80109220_(entry, available);
    return 0;
}

int zz_80053B30_(void *menu, void *entry)
{
    u8 available = 0;
    void *state = zz_80057270_();

    if (state != 0) {
        if ((zz_8011E820_(state) & AGB_STATE_FLAG_1) != 0) {
            available = 1;
        }
    }
    zz_80109220_(entry, available);
    return 0;
}

int zz_80053B98_(void *menu, void *entry)
{
    u8 available = 0;
    void *state = zz_80057270_();

    if (state != 0) {
        if ((zz_8011E820_(state) & AGB_STATE_FLAG_0) != 0) {
            available = 1;
        }
    }
    zz_80109220_(entry, available);
    return 0;
}
