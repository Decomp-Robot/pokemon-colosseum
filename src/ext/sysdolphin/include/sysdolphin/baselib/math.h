#ifndef SYSDOLPHIN_BASELIB_MATH_H
#define SYSDOLPHIN_BASELIB_MATH_H

#include <dolphin/types.h>
#include <libc/msl_math_data.h>
#include <math.h>

#define HSD_PI_F 3.14159265f
#define HSD_PI 3.141592653589793
#define HSD_DEG_TO_RAD_F (HSD_PI_F / 180.0f)

typedef enum _HSD_FloatField {
    HSD_FLOAT_EXPONENT_MASK = 0x7F800000,
    HSD_FLOAT_FRACTION_MASK = 0x007FFFFF
} HSD_FloatField;

static inline long HSD_ClassifyFloat(f32 x)
{
    union {
        f32 f;
        u32 u;
    } bits;
    bits.f = x;
    switch (bits.u & HSD_FLOAT_EXPONENT_MASK) {
    case HSD_FLOAT_EXPONENT_MASK:
        if (bits.u & HSD_FLOAT_FRACTION_MASK)
            return FP_NAN;
        return FP_INFINITE;
    case 0:
        if (bits.u & HSD_FLOAT_FRACTION_MASK)
            return FP_SUBNORMAL;
        return FP_ZERO;
    }
    return FP_NORMAL;
}

static inline f32 HSD_SqrtfPositive(f32 x)
{
    f64 dx = (f64) x;
    f64 guess = __frsqrte(dx);
    const f64 half = 0.5;
    const f64 three = 3.0;
    guess = half * guess * (three - guess * guess * dx);
    guess = half * guess * (three - guess * guess * dx);
    guess = half * guess * (three - guess * guess * dx);
    return (f32) (dx * guess);
}

static inline f32 HSD_Sqrtf(f32 x)
{
    if (x > 0.0f)
        return HSD_SqrtfPositive(x);
    if (x < 0.0)
        return quiet_nan[0];
    if (__fpclassifyf(x) == FP_NAN)
        return quiet_nan[0];
    return x;
}

static inline f32 HSD_SqrtfBits(f32 x)
{
    if (x > 0.0f)
        return HSD_SqrtfPositive(x);
    if (x < 0.0)
        return quiet_nan[0];
    if (HSD_ClassifyFloat(x) == FP_NAN)
        return quiet_nan[0];
    return x;
}

#endif
