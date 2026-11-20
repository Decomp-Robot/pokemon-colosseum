/*
 * Double-precision largest integral value not greater than x.
 *
 * The high and low double words are edited directly.  Adding huge forces
 * the floating-point status word to record an inexact result for fractions.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

static const double
huge =  1.0e300;        /* 0x7E37E43C, 0x8800759C */

double floor(double x)
{
    int i0, i1, j0;
    unsigned i, j;

    i0 = __HI(x);           /* high word of x */
    i1 = __LO(x);           /* low word of x */
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;  /* exponent of x */

    if (j0 < 20) {
        if (j0 < 0) {        /* |x| < 1 */
            if (huge + x > 0.0) {           /* trigger inexact */
                if (i0 >= 0) {
                    i1 = 0;             /* floor of a positive fraction is 0 */
                    i0 = 0;
                } else if (((i0 & 0x7fffffff) | i1) != 0) {
                    i0 = 0xbff00000;    /* floor of a negative fraction is -1 */
                    i1 = 0;
                }
            }
        } else {
            i = 0x000fffff >> j0;
            if (((i0 & i) | i1) == 0) return x;     /* x is integral */
            if (huge + x > 0.0) {           /* trigger inexact */
                if (i0 < 0) i0 += 0x00100000 >> j0;
                i0 &= ~i;
                i1 = 0;
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400) return x + x;  /* inf or NaN */
        else return x;                  /* x is integral */
    } else {
        i = ((unsigned)0xffffffff) >> (j0 - 20);
        if ((i1 & i) == 0) return x;    /* x is integral */
        if (huge + x > 0.0) {           /* trigger inexact */
            if (i0 < 0) {
                if (j0 == 20) i0 += 1;
                else {
                    j = i1 + (1U << (52 - j0));
                    if (j < i1) i0 += 1; /* got a carry */
                    i1 = j;
                }
            }
            i1 &= ~i;
        }
    }
    __HI(x) = i0;
    __LO(x) = i1;
    return x;
}
