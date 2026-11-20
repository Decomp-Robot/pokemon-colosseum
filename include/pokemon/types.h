#ifndef POKEMON_TYPES_H
#define POKEMON_TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed long s32;

/* Type IDs index both axes of the type matchup chart. */
typedef enum TypeId {
    TYPE_NORMAL,
    TYPE_FIGHTING,
    TYPE_FLYING,
    TYPE_POISON,
    TYPE_GROUND,
    TYPE_ROCK,
    TYPE_BUG,
    TYPE_GHOST,
    TYPE_STEEL,
    TYPE_UNKNOWN,
    TYPE_FIRE,
    TYPE_WATER,
    TYPE_GRASS,
    TYPE_ELECTRIC,
    TYPE_PSYCHIC,
    TYPE_ICE,
    TYPE_DRAGON,
    TYPE_DARK,
    TYPE_COUNT,
    TYPE_NONE = TYPE_UNKNOWN,
} TypeId;

typedef enum TypeCategory {
    CATEGORY_NONE,
    CATEGORY_PHYSICAL,
    CATEGORY_SPECIAL,
} TypeCategory;

typedef enum TypeEffectiveness {
    EFF_NORMAL = 63,
    EFF_SUPER = 65,
    EFF_RESIST = 66,
    EFF_IMMUNE = 67,
} TypeEffectiveness;

#endif
