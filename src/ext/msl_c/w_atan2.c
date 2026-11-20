/*
 * Public double-precision atan2 entry point.  The kernel reads the signs of
 * both arguments and returns the angle in the proper quadrant; this member
 * only forwards the call.
 */

#include "__fdlibm.h"

double atan2(double y, double x)
{
    return __ieee754_atan2(y, x);
}
