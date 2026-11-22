/*
 * Accessors for the gamedata attestation BIOS record.
 *
 * The attestation identifiers are unsigned 16-bit values. A missing record
 * is treated as a record of zeroes: the getters return zero and the setters
 * discard the value.
 */
#include "pokemon/types.h"

typedef struct GamedataAttestBios {
    u8 unk_00[1640];
    u16 verId;
    u16 genId;
    u16 areaId;
    u16 langareaId;
} GamedataAttestBios;

typedef struct GamedataBios {
    void *unk_00;
    u16 unk_04;
    u16 unk_06;
    u16 unk_08;
} GamedataBios;

u16 gamedataAttestBiosGetLangareaId(const GamedataAttestBios *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->langareaId;
}

u16 gamedataAttestBiosGetAreaId(const GamedataAttestBios *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->areaId;
}

u16 gamedataAttestBiosGetGenId(const GamedataAttestBios *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->genId;
}

u16 gamedataAttestBiosGetVerId(const GamedataAttestBios *bios)
{
    if (bios == 0) {
        return 0;
    }
    return bios->verId;
}

void gamedataBiosSetUnk08(GamedataBios *bios, u16 value)
{
    if (bios == 0) {
        return;
    }
    bios->unk_08 = value;
}

void gamedataBiosGetGamedataAtttestPtr(GamedataBios *bios, u16 value)
{
    if (bios == 0) {
        return;
    }
    bios->unk_06 = value;
}
