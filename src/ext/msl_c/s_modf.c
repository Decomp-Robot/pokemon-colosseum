/*
 * Double-precision splitting into integral and fractional parts.  The masks
 * applied to the high and low words depend on the exponent; integral results
 * are stored through iptr and a signed zero is returned when appropriate.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])
#define __HIp(x) (((int *)(x))[0])
#define __LOp(x) (((int *)(x))[1])

double modf(double x, double *iptr)
{
    int i0, i1, j0;
    unsigned i;

    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {                          /* integer part in high word */
        if (j0 < 0) {                       /* |x| < 1 */
            __HIp(iptr) = i0 & 0x80000000;
            __LOp(iptr) = 0;
            return x;
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) {
                *iptr = x;
                __HI(x) &= 0x80000000;
                __LO(x) = 0;
                return x;
            } else {
                __HIp(iptr) = i0 & (~i);
                __LOp(iptr) = 0;
                return x - *iptr;
            }
        }
    } else if (j0 > 51) {                   /* no fractional part */
        *iptr = x;
        __HI(x) &= 0x80000000;
        __LO(x) = 0;
        return x;
    } else {                                /* fractional part in low word */
        i = ((unsigned)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) {
            *iptr = x;
            __HI(x) &= 0x80000000;
            __LO(x) = 0;
            return x;
        } else {
            __HIp(iptr) = i0;
            __LOp(iptr) = i1 & (~i);
            return x - *iptr;
        }
    }
}
