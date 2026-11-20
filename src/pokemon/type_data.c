/*
 * Pokemon type metadata and type-matchup rules.
 *
 * Every record holds the category, icon and message IDs used by the menus,
 * followed by that type's effectiveness against each defending type.
 * Both row and column order are TypeId order, TYPE_NORMAL through TYPE_DARK.
 */
#include "pokemon/types.h"

typedef struct {
    u8 category;                   /* TypeCategory */
    u16 icon;
    u32 nameId;
    u16 effectiveness[TYPE_COUNT]; /* TypeEffectiveness */
} TypeRecord;

typedef struct {
    u32 count;
    u32 unused;
} TypeInfo;

TypeInfo gTypeInfo = { TYPE_COUNT, 0 };

static TypeRecord sTypeChart[TYPE_COUNT] = {
    /* TYPE_NORMAL */ { CATEGORY_PHYSICAL, 86, 3001, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_RESIST,
        EFF_NORMAL, EFF_IMMUNE, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_FIGHTING */ { CATEGORY_PHYSICAL, 88, 3007, {
        EFF_SUPER, EFF_NORMAL, EFF_RESIST, EFF_RESIST, EFF_NORMAL, EFF_SUPER,
        EFF_RESIST, EFF_IMMUNE, EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_SUPER, EFF_NORMAL, EFF_SUPER,
    } },
    /* TYPE_FLYING */ { CATEGORY_PHYSICAL, 84, 3010, {
        EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_RESIST,
        EFF_SUPER, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_SUPER, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_POISON */ { CATEGORY_PHYSICAL, 76, 3008, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_RESIST, EFF_RESIST,
        EFF_NORMAL, EFF_RESIST, EFF_IMMUNE, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_GROUND */ { CATEGORY_PHYSICAL, 90, 3009, {
        EFF_NORMAL, EFF_NORMAL, EFF_IMMUNE, EFF_SUPER, EFF_NORMAL, EFF_SUPER,
        EFF_RESIST, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_SUPER, EFF_NORMAL,
        EFF_RESIST, EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_ROCK */ { CATEGORY_PHYSICAL, 81, 3013, {
        EFF_NORMAL, EFF_RESIST, EFF_SUPER, EFF_NORMAL, EFF_RESIST, EFF_NORMAL,
        EFF_SUPER, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_SUPER, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_BUG */ { CATEGORY_PHYSICAL, 85, 3012, {
        EFF_NORMAL, EFF_RESIST, EFF_RESIST, EFF_RESIST, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_RESIST, EFF_RESIST, EFF_NORMAL, EFF_RESIST, EFF_NORMAL,
        EFF_SUPER, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_SUPER,
    } },
    /* TYPE_GHOST */ { CATEGORY_PHYSICAL, 82, 3014, {
        EFF_IMMUNE, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_SUPER, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_RESIST,
    } },
    /* TYPE_STEEL */ { CATEGORY_PHYSICAL, 80, 3017, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_SUPER,
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_RESIST, EFF_RESIST,
        EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_UNKNOWN */ { CATEGORY_NONE, 93, 3018, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
    } },
    /* TYPE_FIRE */ { CATEGORY_SPECIAL, 89, 3002, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_RESIST,
        EFF_SUPER, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_RESIST, EFF_RESIST,
        EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_RESIST, EFF_NORMAL,
    } },
    /* TYPE_WATER */ { CATEGORY_SPECIAL, 91, 3003, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_SUPER,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_RESIST,
        EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL,
    } },
    /* TYPE_GRASS */ { CATEGORY_SPECIAL, 92, 3005, {
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_RESIST, EFF_SUPER, EFF_SUPER,
        EFF_RESIST, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_RESIST, EFF_SUPER,
        EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL,
    } },
    /* TYPE_ELECTRIC */ { CATEGORY_SPECIAL, 79, 3004, {
        EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_IMMUNE, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_SUPER,
        EFF_RESIST, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL,
    } },
    /* TYPE_PSYCHIC */ { CATEGORY_SPECIAL, 83, 3011, {
        EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_IMMUNE,
    } },
    /* TYPE_ICE */ { CATEGORY_SPECIAL, 77, 3006, {
        EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_SUPER, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_RESIST, EFF_RESIST,
        EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_SUPER, EFF_NORMAL,
    } },
    /* TYPE_DRAGON */ { CATEGORY_SPECIAL, 78, 3015, {
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_NORMAL,
    } },
    /* TYPE_DARK */ { CATEGORY_SPECIAL, 87, 3016, {
        EFF_NORMAL, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_SUPER, EFF_RESIST, EFF_NORMAL, EFF_NORMAL, EFF_NORMAL,
        EFF_NORMAL, EFF_NORMAL, EFF_SUPER, EFF_NORMAL, EFF_NORMAL, EFF_RESIST,
    } },
};

#pragma optimization_level 0
u16 typeGetIconId(u16 type)
{
    if (type >= gTypeInfo.count) {
        return 0;
    }
    return sTypeChart[type].icon;
}

u8 typeGetCategory(u16 type)
{
    if (type >= gTypeInfo.count) {
        return 0;
    }
    return sTypeChart[type].category;
}

u32 typeGetNameId(u16 type)
{
    if (type >= gTypeInfo.count) {
        return 0;
    }
    return sTypeChart[type].nameId;
}

#pragma dont_inline on
u16 typeGetEffectiveness(u32 type, u32 againstType)
{
    if (type >= gTypeInfo.count) {
        return 0;
    }
    if (againstType >= TYPE_COUNT) {
        return 0;
    }
    return sTypeChart[type].effectiveness[againstType];
}

s32 typeGetMatchupScore(u16 type, u8 attacking)
{
    s32 score = 0;
    u16 i;
    u16 code;

    for (i = 0; i < gTypeInfo.count; i++) {
        if (i == TYPE_NONE) {
            continue;
        }
        if (attacking == 1) {
            u16 effectiveness = typeGetEffectiveness(type, i);
            code = effectiveness;
            if (code == EFF_IMMUNE) {
                score -= 10;
            } else if (code == EFF_RESIST) {
                score -= 10;
            } else if (code == EFF_SUPER) {
                score += 10;
            }
        } else {
            u16 effectiveness = typeGetEffectiveness(i, type);
            code = effectiveness;
            if (code == EFF_IMMUNE) {
                score += 10;
            } else if (code == EFF_RESIST) {
                score += 10;
            } else if (code == EFF_SUPER) {
                score -= 10;
            }
        }
    }
    return score;
}

u16 typeGetCombinedEffectiveness(u16 type, u16 *types, u16 count)
{
    u16 i;
    u16 super = 0;
    u16 resist = 0;

    for (i = 0; i < count; i++) {
        u16 attackingType;
        u16 effectiveness;
        u16 code;

        if (i == TYPE_NONE) {
            continue;
        }
        attackingType = type;
        effectiveness = typeGetEffectiveness(attackingType, types[i]);
        code = effectiveness;
        if (code == EFF_IMMUNE) {
            return EFF_IMMUNE;
        }
        if (code == EFF_SUPER) {
            super++;
        } else if (code == EFF_RESIST) {
            resist++;
        }
    }
    if (super == 0 && resist == 0) {
        return EFF_NORMAL;
    }
    if (super == resist) {
        return EFF_NORMAL;
    }
    if (super > resist) {
        return EFF_SUPER;
    }
    return EFF_RESIST;
}

u16 typeGetEffectivenessAgainst(u16 type, u16 againstType)
{
    return typeGetEffectiveness(type, againstType);
}
