/*
 * Debug menu commands for the hero's items and party.
 *
 * Command handlers return DBG_MENU_CLOSE when the menu should close and
 * DBG_MENU_STAY_OPEN when it should stay open. Pokemon selection reads the
 * party slot from the hero's trainer data and passes the chosen Pokemon to
 * the shared battle setup routine.
 */
#include "app/menu/dbgMenu.h"
#include "app/menu/menuWindow.h"
#include "app/pokemon/pokemon.h"
#include "pokemon/types.h"

enum DbgMenuResult {
    DBG_MENU_STAY_OPEN = 0,
    DBG_MENU_CLOSE = -1,
};

#define HERO_TRAINER_INDEX 0
#define TRAINER_VALUE_PARTY_POKEMON 3

extern void zz_8005DEE8_(void);
extern void zz_801293FC_(int trainer, int value);
extern void zz_801294C4_(int trainer, int value);
extern void zz_battle_pokemon_init(Pokemon *pokemon);
extern int zz_800096B4_(Pokemon *pokemon, int, int, int, int, int);
extern void zz_80129F20_(int trainer, Pokemon *pokemon, int, int, int);
extern Pokemon *zz_trainer_get_value_with_index(int trainer, int kind, int index);

Pokemon dbgPokemonWork;
u8 dbgPokemonWorkInit = 1;

int dbgMenuHeroCreateItem(void)
{
    zz_8005DEE8_();
    return DBG_MENU_STAY_OPEN;
}

int dbgMenuHeroIncCoupon(void)
{
    int coupon = menuWindowOpen(MENU_WINDOW_DEBUG_INPUT, 1);

    if (coupon == -1) {
        return DBG_MENU_STAY_OPEN;
    }
    zz_801293FC_(HERO_TRAINER_INDEX, coupon);
    return DBG_MENU_STAY_OPEN;
}

int dbgMenuHeroIncMoney(void)
{
    int money = menuWindowOpen(MENU_WINDOW_DEBUG_INPUT, 1);

    if (money == -1) {
        return DBG_MENU_STAY_OPEN;
    }
    zz_801294C4_(HERO_TRAINER_INDEX, money);
    return DBG_MENU_STAY_OPEN;
}

int dbgMenuHeroPokemonAdd(void)
{
    if (dbgTriggerIsEnabled() == 1) {
        return DBG_MENU_CLOSE;
    }
    if (dbgPokemonWorkInit) {
        zz_battle_pokemon_init(&dbgPokemonWork);
        dbgPokemonWorkInit = 0;
    }
    if (zz_800096B4_(&dbgPokemonWork, 0, 0, 0, 0, 0) < 0) {
        return DBG_MENU_CLOSE;
    }
    zz_80129F20_(HERO_TRAINER_INDEX, &dbgPokemonWork, 0, 4, 1);
    return DBG_MENU_CLOSE;
}

static inline int selectPokemon(int slot)
{
    Pokemon *pokemon;

    if (dbgTriggerIsEnabled() == 1) {
        return DBG_MENU_CLOSE;
    }
    pokemon = zz_trainer_get_value_with_index(HERO_TRAINER_INDEX, TRAINER_VALUE_PARTY_POKEMON, slot);
    if (pokemon == 0) {
        return DBG_MENU_CLOSE;
    }
    return zz_800096B4_(pokemon, 0, 0, 0, 0, 0);
}

int dbgMenuHeroPokemonSelect5(void)
{
    return selectPokemon(5);
}

int dbgMenuHeroPokemonSelect4(void)
{
    return selectPokemon(4);
}

int dbgMenuHeroPokemonSelect3(void)
{
    return selectPokemon(3);
}

int dbgMenuHeroPokemonSelect2(void)
{
    return selectPokemon(2);
}

int dbgMenuHeroPokemonSelect1(void)
{
    return selectPokemon(1);
}

int dbgMenuHeroPokemonSelect0(void)
{
    return selectPokemon(0);
}
