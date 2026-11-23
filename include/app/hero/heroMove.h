#ifndef APP_HERO_HEROMOVE_H
#define APP_HERO_HEROMOVE_H

#include "pokemon/types.h"

/*
 * Party scan: *slot is the party cursor to continue from.  The cursor is
 * advanced over the party entries that match, and the Pokemon the scan
 * stopped on is returned, or null once the cursor has passed the six party
 * slots.
 */
void *zz_801F8A18_(void *data, u16 *slot);

#endif
