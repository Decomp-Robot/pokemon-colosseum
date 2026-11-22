/*
 * Accessors for the per-move BIOS record's flag word.
 *
 * A missing record is treated as a record with no flags: the mask operations
 * do nothing and the getter returns 0.
 */
#include "pokemon/types.h"

typedef struct WazaDataBios {
    void *unk_00;
    u32 flags;
    void *unk_08;
    u32 unk_0C[3];
    void *unk_18;
    void *unk_1C;
    void *unk_20;
} WazaDataBios;

void wazaDataBiosGetMamoru(WazaDataBios *bios, u32 mask)
{
    if (bios == 0) {
        return;
    }
    bios->flags &= ~mask;
}

void wazaDataBiosGetDageki(WazaDataBios *bios, u32 mask)
{
    if (bios == 0) {
        return;
    }
    bios->flags |= mask;
}

u32 wazaDataBiosGetAddFightKoukaAvg(const WazaDataBios *bios)
{
    if (bios != 0) {
        return bios->flags;
    }
    return 0;
}
