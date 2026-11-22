/*
 * Double-precision cosine.
 *
 * Arguments inside pi/4 go straight to the kernel.  Larger finite arguments
 * are reduced to a quadrant count and a small remainder; the quadrant selects
 * between the sine and cosine kernels and the sign of their result.  Infinity
 * and NaN have no cosine, so they produce a NaN via x-x.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])

static const double zero = 0.0;

double cos(double x)
{
    double y[2];
    int n, ix;

    ix = __HI(x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) return __kernel_cos(x, zero);
    else if (ix >= 0x7ff00000) return x-x;
    else {
        n = __ieee754_rem_pio2(x, y);
        switch (n & 3) {
        case 0: return  __kernel_cos(y[0], y[1]);
        case 1: return -__kernel_sin(y[0], y[1], 1);
        case 2: return -__kernel_cos(y[0], y[1]);
        default: return  __kernel_sin(y[0], y[1], 1);
        }
    }
}
