#include "app/floor/floorCommand.h"
#include "app/message/message.h"
#include "app/pokemon/pokemon.h"
#include "app/pokemon/pokemonStats.h"
#include "pokemon/types.h"

/* Levels from 100 up need a third digit in the level line. */
#define LEVEL_THIRD_DIGIT_MIN 100

/* Pixel width of one digit of the message font. */
#define MESSAGE_DIGIT_WIDTH 15

extern Pokemon *zz_80057270_(void);
extern void zz_80109220_(FloorCommandEntry *entry, int available);
extern u16 zz_801230E0_(Pokemon *pokemon);
extern u8 zz_8001DA60_(Pokemon *pokemon);
extern u16 zz_8011F5C8_(Pokemon *pokemon);
extern u8 zz_8011F4A8_(Pokemon *pokemon);
extern PokemonStats *zz_pokemon_stats_get_stats_for_species_with_id(u16 species);
extern u32 zz_8011E760_(const PokemonStats *stats);
extern char *zz_get_string_with_msg_id(u32 msgId);
extern s32 zz_msg_window_draw(s32 x, s32 y, s32 color, MessageId msgId);
extern u32 zz_800FA444_(u32 msgId);

int floorReadSetAvailable(void *menu, FloorCommandEntry *entry)
{
    u8 available = 0;
    Pokemon *pokemon = zz_80057270_();

    if (pokemon != 0) {
        if (zz_801230E0_(pokemon) != 0) {
            available = 1;
        }
    }
    zz_80109220_(entry, available);
    return 0;
}

int floorReadSetGenderMessage(void *menu, FloorCommandEntry *entry)
{
    MessageId messageId = MESSAGE_ID_NONE;
    Pokemon *pokemon = zz_80057270_();

    if (pokemon != 0) {
        switch (zz_8001DA60_(pokemon)) {
        case POKEMON_GENDER_MALE:
            messageId = MESSAGE_ID_FLOOR_READ_GENDER_MALE;
            break;
        case POKEMON_GENDER_FEMALE:
            messageId = MESSAGE_ID_FLOOR_READ_GENDER_FEMALE;
            break;
        case POKEMON_GENDER_GENDERLESS:
            break;
        }
    }
    entry->messageId = messageId;
    return 0;
}

int floorReadShowSpeciesName(void *menu, FloorCommandEntry *entry)
{
    Pokemon *pokemon = zz_80057270_();

    if (pokemon != 0) {
        setMessageVariable(MESSAGE_VARIABLE_FLOOR_READ_SPECIES_NAME,
                       (u32)zz_get_string_with_msg_id(zz_8011E760_(
                           zz_pokemon_stats_get_stats_for_species_with_id(zz_8011F5C8_(pokemon)))));
        zz_msg_window_draw(0, 0, -1, MESSAGE_ID_FLOOR_READ_SPECIES);
    }
    entry->messageId = MESSAGE_ID_NONE;
    return 0;
}

int floorReadShowLevel(void *menu, FloorCommandEntry *entry)
{
    int level;
    int digits;
    s32 offset;
    MessageId messageId = MESSAGE_ID_NONE;
    Pokemon *pokemon = zz_80057270_();

    if (pokemon != 0) {
        level = zz_8011F4A8_(pokemon);
        if (level < LEVEL_THIRD_DIGIT_MIN) {
            digits = 2;
        } else {
            digits = 3;
        }
        offset = zz_800FA444_(MESSAGE_ID_FLOOR_READ_LEVEL) >> 16;
        zz_msg_window_draw(entry->textX - digits * MESSAGE_DIGIT_WIDTH - offset, 0, -1,
                           MESSAGE_ID_FLOOR_READ_LEVEL);
        setMessageVariable(MESSAGE_VARIABLE_FLOOR_READ_LEVEL, level);
        messageId = MESSAGE_ID_FLOOR_READ_RESULT;
    }
    entry->messageId = messageId;
    return 0;
}
