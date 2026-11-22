/*
 * Front ends for the Pokemon menu. Each one records how the menu was entered
 * and then passes the initial panel and selection to the menu dispatcher.
 */
#include "pokemon/types.h"

/*
 * State shared with the menu dispatcher. The fields not touched here are used
 * by the individual panels while the menu is open.
 */
typedef struct PokemonMenuState {
    u8 mode;
    u8 flags;
    u8 panel;
    u8 subPanel;
    signed char itemMode;
    signed char field5;
    signed char field6;
    signed char field7;
    u32 field8;
    u32 selectionId;
    u8 source;
    u8 destination;
    u16 index;
    u8 field14;
    u8 field15;
    u32 fightData;
    u32 result;
} PokemonMenuState;

extern PokemonMenuState lbl_803A1D40;

enum PokemonMenuMode {
    MENU_MODE_FIGHT = 2,
    MENU_MODE_GIVE_ITEM = 5,
};

extern int zz_8001BAC4_(u8 mode, u8 source, u8 destination, u16 index, u32 selectionId, u8 flags);

int menuPokemonOpenItemGive(u8 item, u8 from, u16 count, u32 data)
{
    lbl_803A1D40.itemMode = 1;
    return zz_8001BAC4_(MENU_MODE_GIVE_ITEM, item, from, count, data, 1);
}

int menuPokemonOpenItemUse(u8 mode, u8 useFlag, u16 item, u32 data)
{
    lbl_803A1D40.itemMode = 1;
    return zz_8001BAC4_(mode, 0, 0, item, data, useFlag);
}

int menuPokemonOpenFight(u8 battleKind, u8 slot, u32 pokemon, u32 data)
{
    if (battleKind == 1) {
        lbl_803A1D40.itemMode = 0;
    } else {
        lbl_803A1D40.itemMode = 1;
    }
    lbl_803A1D40.fightData = data;
    return zz_8001BAC4_(MENU_MODE_FIGHT, 0, 0, slot, pokemon, 1);
}

int menuPokemonOpen(u8 mode, u16 source, u32 index)
{
    lbl_803A1D40.itemMode = 1;
    return zz_8001BAC4_(mode, 0, 0, source, index, 1);
}
