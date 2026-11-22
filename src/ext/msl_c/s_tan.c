/*
 * Double-precision tangent.
 *
 * The pi/4 cutoff sends small arguments directly to the kernel in its result
 * mode.  Beyond it the reducer supplies a small remainder and a quadrant
 * count; the quadrant's parity selects the kernel's reciprocal mode.
 * Non-finite inputs have no tangent and return NaN.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])

static const double zero = 0.0;

double tan(double x)
{
    double y[2];
    int n, ix;

    ix = __HI(x);
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) return __kernel_tan(x, zero, 1);
    if (ix >= 0x7ff00000) return x-x;
    n = __ieee754_rem_pio2(x, y);
    return __kernel_tan(y[0], y[1], 1 - 2 * (n & 1));
}
