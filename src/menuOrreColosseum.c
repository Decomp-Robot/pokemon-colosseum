/*
 * Colosseum menu helpers.
 *
 * The deck pickers draw from the shared random-number state and reduce the
 * result to the number of choices supplied by the caller.  The face-ID
 * accessor latches the low half of the system time into the menu's shared
 * seed/time word.  The last helper folds an angle into a half-degree cosine
 * lookup and applies the sign of the half-turn the angle came from.
 */
#include "pokemon/types.h"

typedef unsigned long long u64;

extern u32 zz_RNG(void);
extern float zz_801ADC7C_(void);
extern u64 OSGetTime(void);
extern double fmod(double value, double divisor);

/* Shared RNG/time word. */
extern u32 *lbl_80478C94;

extern const float sFoldAngle;
extern const float sPositiveSign;
extern const float sHalfTurnAngle;
extern const float sNegativeSign;
extern const double sAnglePeriod;
extern const float sIndexBias;
extern const float sIndexScale;

/* Cosine lookup filled at startup, one entry per half degree. */
extern float sCosineTable[];

float zz_800E0BE4_(void)
{
    return zz_801ADC7C_();
}

u32 GetDeckPokemonPtr__8menuOrreFUs(u32 count)
{
    u32 low = zz_RNG();
    u32 shifted = zz_RNG() << 16;

    return (shifted | low) % count;
}

u16 GetTemochiPokemonName__8menuOrreFUs(u16 index)
{
    return zz_RNG() & 0xFFFF;
}

void GetTemochiPokemonFaceID__8menuOrreFUs(u16 index)
{
    *lbl_80478C94 = OSGetTime();
}

float zz_800E0CA0_(float angle)
{
    float sign = sPositiveSign;

    if (angle > sHalfTurnAngle) {
        sign = sNegativeSign;
        angle = fmod(angle, sAnglePeriod);
    }
    if (angle > sFoldAngle) {
        angle = sHalfTurnAngle - angle;
    }
    return sign * sCosineTable[(u32)(sIndexScale * angle + sIndexBias)];
}
