#include "app/fight/fightAiData.h"
#include "pokemon/types.h"

u8 memcardMemFreeForSavedata(void *fight, void *pokemon)
{
    return zz_fight_ai_data_get_pokemon_value(fight, pokemon) == 1;
}
