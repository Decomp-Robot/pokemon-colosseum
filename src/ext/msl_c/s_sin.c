/*
 * Double-precision sine.
 *
 * Small arguments are handed to the sine kernel with a zero tail.  For larger
 * arguments the quadrant count from the reducer chooses the sine or cosine
 * kernel and whether the result is negated.  Non-finite inputs yield NaN.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])

static const double zero = 0.0;

double sin(double x)
{
    double y[2];
    int n, ix;

    ix = __HI(x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) return __kernel_sin(x, zero, 0);
    else if (ix >= 0x7ff00000) return x-x;
    else {
        n = __ieee754_rem_pio2(x, y);
        switch (n & 3) {
        case 0: return  __kernel_sin(y[0], y[1], 1);
        case 1: return  __kernel_cos(y[0], y[1]);
        case 2: return -__kernel_sin(y[0], y[1], 1);
        default: return -__kernel_cos(y[0], y[1]);
        }
    }
}
