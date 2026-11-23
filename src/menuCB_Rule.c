/*
 * Battle-rule menu callbacks.
 *
 * Callers pass a one-based rule index; the helpers below receive index - 1.
 * The first two callbacks update a per-rule retry counter from the helper result.
 */
#include "pokemon/types.h"

/* Peephole scheduling is disabled across this rule-menu module. */
#pragma peephole off

extern s32 zz_800719A8_(s32 index);
extern s32 zz_80071AE4_(s32 index, void *menuData);
extern s32 zz_800722A0_(s32 index);

/* Per-rule retry counters, stored zero-based (callers pass a one-based index). */
extern u16 ruleRetryCount[3];

/* Retry while the per-rule count is at most this value. */
#define RULE_RETRY_LIMIT 10

s32 zz_80089CA8_(s32 item)
{
    s32 result = zz_800719A8_(item - 1);

    if (result < 0) {
        ruleRetryCount[item - 1] = 0;
    } else if (result == 1 || result == 2) {
        if (++ruleRetryCount[item - 1] <= RULE_RETRY_LIMIT) {
            return -1;
        }
    }
    return result;
}

s32 zz_80089D30_(s32 item, void *menuData)
{
    s32 result = zz_80071AE4_(item - 1, menuData);

    ruleRetryCount[item - 1] = 0;
    return result;
}

s32 menuCBRule_EnableSkill_SonicBoom(s32 item)
{
    return zz_800722A0_(item - 1);
}
