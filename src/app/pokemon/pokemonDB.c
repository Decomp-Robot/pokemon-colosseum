/*
 * Accessors for the battle-result Pokemon report.
 *
 * A null report is fetched from the current trainer with
 * zz_trainer_get_data_pointer(0, 11): trainer data type 11 selects the
 * battle-result Pokemon report block.
 */
#include "pokemon/types.h"

typedef struct FightOutPokemonReport {
    u8 targetId;
    u8 damage;
    u16 joutaiDataId;
    s32 iryoku;
    u8 isHit[4];
} FightOutPokemonReport;

extern void *zz_trainer_get_data_pointer(void *trainer, u16 dataType);

static inline FightOutPokemonReport *fightOutPokemon_GetReport(FightOutPokemonReport *report)
{
    if (report == 0) {
        report = zz_trainer_get_data_pointer(0, 11);
    }
    return report;
}

u8 fightOutPokemon_GetTargetId(FightOutPokemonReport *report)
{
    if (report == 0) {
        report = zz_trainer_get_data_pointer(0, 11);
    }
    return report->targetId;
}

s32 fightOutPokemon_GetIryoku(FightOutPokemonReport *report)
{
    if (report == 0) {
        report = zz_trainer_get_data_pointer(0, 11);
    }
    if (report->targetId != 0) {
        return report->iryoku;
    }
    return -1;
}

u8 fightOutPokemon_GetDamage(FightOutPokemonReport *report)
{
    if (report == 0) {
        report = zz_trainer_get_data_pointer(0, 11);
    }
    if (report->targetId != 0) {
        return report->damage;
    }
    return 0xFF;
}

u8 *fightOutPokemon_GetIsHit(FightOutPokemonReport *report)
{
    if (report == 0) {
        report = zz_trainer_get_data_pointer(0, 11);
    }
    return &report->isHit[0];
}

void fightOutPokemon_GetJoutaiDataId(FightOutPokemonReport *report)
{
    FightOutPokemonReport *joutai;

    report = fightOutPokemon_GetReport(report);
    if (report == 0) {
        return;
    }
    joutai = fightOutPokemon_GetReport(report);
    if (joutai != 0) {
        joutai->joutaiDataId = 0;
    }
    report->damage = 0;
    report->targetId = 0;
}
