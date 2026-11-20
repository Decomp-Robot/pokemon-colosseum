/*
 * Public double-precision natural logarithm entry point.  The kernel carries
 * all of the special-case and range-reduction work; this member only forwards
 * the call.
 */

#include "__fdlibm.h"

double log(double x)
{
    return __ieee754_log(x);
}
