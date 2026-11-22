/*
 * Debug menu commands for editing a fight's trainer data.
 *
 * The fight menu edits the selected trainer through a panel driven by the
 * selected field. Changing a combo, item, move or swap passes the edit to
 * the matching add/subtract routine in the fight data code; the panel only
 * runs while the debug trigger flag is set.
 */
#include "pokemon/types.h"

/* Pokemon data selectors used to reach the trainer-AI edit field. */
#define FIGHT_AI_DATA_SELECTOR 67
#define FIGHT_AI_FIELD_SELECTOR 2

/* Shared runtime state holding the pointer to the selected fight data. */
extern void *lbl_8047A278;

extern int zz_pokemon_data_get_value_with_index_r5(void *data, int index, int selector, int value);
extern u8 zz_801EF63C_(void);
extern int zz_80051E38_(int value);

extern int zz_8004EADC_(void);
extern int zz_8004EC54_(void);
extern int zz_8004EDCC_(void);
extern int zz_8004F860_(void);
extern int zz_8004FE3C_(void);
extern int zz_80050844_(void);

int dbgMenuFightFightTrainerAiDataEdit(void)
{
    u16 trainer;
    u16 field;

    trainer = zz_pokemon_data_get_value_with_index_r5(lbl_8047A278, 0, FIGHT_AI_DATA_SELECTOR, 0);
    field = zz_pokemon_data_get_value_with_index_r5(0, trainer, FIGHT_AI_FIELD_SELECTOR, 0);

    if (zz_801EF63C_() == 0) {
        return -1;
    }
    return zz_80051E38_(field);
}

int dbgMenuFightFightTrainerAiAddsubCombo(void)
{
    return zz_8004EADC_();
}

int dbgMenuFightFightTrainerAiAddsubItem(void)
{
    return zz_8004EC54_();
}

int dbgMenuFightFightTrainerAiAddsubWazaUsewaza(void)
{
    return zz_8004EDCC_();
}

int dbgMenuFightFightTrainerAiAddsubWazaDefpokemon(void)
{
    return zz_8004F860_();
}

int dbgMenuFightFightTrainerAiAddsubWaza(void)
{
    return 1;
}

int dbgMenuFightFightTrainerAiAddsubIrekaeDasu(void)
{
    return zz_8004FE3C_();
}

int dbgMenuFightFightTrainerAiAddsubIrekaeModosu(void)
{
    return zz_80050844_();
}

int dbgMenuFightFightTrainerAiAddsubIrekae(void)
{
    return 1;
}
