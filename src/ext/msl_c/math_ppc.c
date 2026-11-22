/*
 * PowerPC math support from the Metrowerks C library.
 *
 * The double-precision square root is the portable, correctly rounded,
 * bit-by-bit restoring kernel.  The single-precision side uses the Gekko
 * reciprocal-square-root estimate followed by three Newton refinements;
 * exceptional single-precision arguments are separated with the IEEE float
 * classifier.  The remaining members are the thin call-and-round float
 * wrappers around the double trigonometric entry points and the double
 * square-root forwarder.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

extern int errno;
extern float quiet_nan[];

double tan(double), sin(double), cos(double), acos(double);

#define EDOM 33

/* IEEE classifications returned by the float classifier below. */
#define FP_NAN       1
#define FP_INFINITE  2
#define FP_ZERO      3
#define FP_NORMAL    4
#define FP_SUBNORMAL 5

static const double one = 1.0, tiny = 1.0e-300;

/* Inline float classification for the square root's NaN test. */
static inline long classify_float(float x)
{
    union { float f; unsigned int u; } ux;
    long cls;
    ux.f = x;
    switch (ux.u & 0x7f800000) {
    case 0x7f800000:
        if (ux.u & 0x007fffff) cls = FP_NAN;
        else cls = FP_INFINITE;
        break;
    case 0:
        if (ux.u & 0x007fffff) cls = FP_SUBNORMAL;
        else cls = FP_ZERO;
        break;
    default:
        cls = FP_NORMAL;
        break;
    }
    return cls;
}

double __ieee754_sqrt(double x)
{
    double z;
    int sign = (int)0x80000000;
    int ix0;
    unsigned int t1;
    unsigned int s1;
    int m;
    int t;
    int i;
    unsigned int r;
    unsigned int q1;
    int s0;
    int q;
    unsigned int ix1;

    ix0 = __HI(x);
    ix1 = __LO(x);

    /* A NaN or infinity propagates through x*x+x. */
    if ((ix0 & 0x7ff00000) == 0x7ff00000) {
        errno = EDOM;
        return x*x+x;
    }
    /* Zero keeps its sign; a negative argument is a domain error. */
    if (ix0 <= 0) {
        if (((ix0 & ~sign) | ix1) == 0) return x;
        else if (ix0 < 0) {
            errno = EDOM;
            return quiet_nan[0];
        }
    }

    /* Scale by an even power of two so that the significand lies in [1,4). */
    m = (ix0 >> 20);
    if (m == 0) {                 /* subnormal argument */
        while (ix0 == 0) {
            m -= 21;
            ix0 |= (ix1 >> 11);
            ix1 <<= 21;
        }
        for (i = 0; (ix0 & 0x00100000) == 0; i++) ix0 <<= 1;
        m -= i - 1;
        ix0 |= (ix1 >> (32 - i));
        ix1 <<= i;
    }
    m -= 1023;
    ix0 = (ix0 & 0x000fffff) | 0x00100000;
    if (m & 1) {                  /* make the exponent even */
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
    }
    m >>= 1;

    /* Generate the root one bit at a time. */
    ix0 += ix0 + ((ix1 & sign) >> 31);
    ix1 += ix1;
    s1 = 0;
    s0 = 0;
    q1 = 0;
    q = 0;
    r = 0x00200000;
    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    r = sign;
    while (r != 0) {
        t1 = s1 + r;
        t = s0;
        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1 = t1 + r;
            if (((t1 & sign) == sign) && (s1 & sign) == 0) s0 += 1;
            ix0 -= t;
            if (ix1 < t1) ix0 -= 1;
            ix1 -= t1;
            q1 += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    /* Round according to the remainder left in the two working words. */
    if ((ix0 | ix1) != 0) {
        z = one-tiny;             /* trigger the inexact flag */
        if (z >= one) {
            z = one+tiny;
            if (q1 == (unsigned int)0xffffffff) { q1 = 0; q += 1; }
            else if (z > one) {
                if (q1 == (unsigned int)0xfffffffe) q += 1;
                q1 += 2;
            } else
                q1 += (q1 & 1);
        }
    }
    ix0 = (q >> 1) + 0x3fe00000;
    ix1 = q1 >> 1;
    if ((q & 1) == 1) ix1 |= sign;
    ix0 += (m << 20);
    __HI(z) = ix0;
    __LO(z) = ix1;
    return z;
}

/* Double-precision absolute value: a single floating-point instruction. */
double fabs(double x)
{
    return __fabs(x);
}

float sqrtf(float x)
{
    static const double _half = .5;
    static const double _three = 3.0;

    if (x > 0.0f) {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        return (float)(x*guess);
    }
    if (x < 0.0) return quiet_nan[0];
    if (classify_float(x) == FP_NAN) return quiet_nan[0];
    return x;
}

float tanf(float x)
{
    return (float)tan(x);
}

float sinf(float x)
{
    return (float)sin(x);
}

float cosf(float x)
{
    return (float)cos(x);
}

float acosf(float x)
{
    return (float)acos(x);
}

/* IEEE classification of a single-precision value. */
long __fpclassifyf(float x)
{
    union { float f; unsigned int u; } ux;

    ux.f = x;
    switch (ux.u & 0x7f800000) {
    case 0x7f800000:
        return (ux.u & 0x007fffff) ? FP_NAN : FP_INFINITE;
    case 0:
        return (ux.u & 0x007fffff) ? FP_SUBNORMAL : FP_ZERO;
    default:
        return FP_NORMAL;
    }
}

double sqrt(double x)
{
    return __ieee754_sqrt(x);
}
