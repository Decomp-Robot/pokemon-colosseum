/*
 * Copy the sign of one double onto the magnitude of another.  The sign bit
 * lives in the high word of the double and the low word is untouched.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])

double copysign(double x, double y)
{
    __HI(x) = (__HI(x) & 0x7fffffff) | (__HI(y) & 0x80000000);
    return x;
}
