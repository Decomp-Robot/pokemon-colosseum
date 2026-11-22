/*
 * Double-precision scaling by a power of two.
 *
 * The exponent field is edited directly so ordinary results are exact.  The
 * huge and tiny products on the boundary paths force overflow to an infinity
 * and underflow to a signed zero.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

enum FPClass {
    FP_CLASS_NAN = 1,
    FP_CLASS_INFINITE = 2,
    FP_CLASS_ZERO = 3,
    FP_CLASS_NORMAL = 4,
    FP_CLASS_SUBNORMAL = 5
};

static inline int classify(double x)
{
    switch (__HI(x) & 0x7ff00000) {
    case 0x7ff00000:
        if ((__HI(x) & 0x000fffff) || __LO(x))
            return FP_CLASS_NAN;
        else
            return FP_CLASS_INFINITE;
    case 0:
        if ((__HI(x) & 0x000fffff) || __LO(x))
            return FP_CLASS_SUBNORMAL;
        else
            return FP_CLASS_ZERO;
    }
    return FP_CLASS_NORMAL;
}

#define is_finite(x) (classify(x) > FP_CLASS_INFINITE)

static const double two54 = 1.80143985094819840000e+16; /* 2^54 */
static const double tiny = 1.0e-300;
static const double huge = 1.0e300;
static const double twom54 = 5.55111512312578270212e-17; /* 2^-54 */

double ldexp(double x, int n)
{
    int k, hx, lx;

    if (!is_finite(x) || x == 0.0) return x;
    hx = __HI(x);
    lx = __LO(x);
    k = (hx & 0x7ff00000) >> 20;
    if (k == 0) {                            /* zero or subnormal */
        if ((lx | (hx & 0x7fffffff)) == 0) return x;
        x *= two54;
        hx = __HI(x);
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (n < -50000) return tiny * x;
    }
    if (k == 0x7ff) return x + x;
    k = k + n;
    if (k > 0x7fe) return huge * copysign(huge, x);
    if (k > 0) {
        __HI(x) = (hx & 0x800fffff) | (k << 20);
        return x;
    }
    if (k <= -54) {
        if (n > 50000)
            return huge * copysign(huge, x);
        else
            return tiny * copysign(tiny, x);
    }
    k += 54;
    __HI(x) = (hx & 0x800fffff) | (k << 20);
    return x * twom54;
}
