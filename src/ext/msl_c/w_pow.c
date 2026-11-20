/*
 * Public double-precision power entry point.  The kernel carries all of the
 * special-case and range-reduction work; this member only forwards the call.
 */

#include "__fdlibm.h"

double pow(double x, double y)
{
    return __ieee754_pow(x, y);
}
