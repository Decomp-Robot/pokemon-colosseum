/*
 * Public double-precision exponential entry point.  The kernel performs the
 * argument reduction and all special-case handling; this member forwards the
 * call.
 */

#include "__fdlibm.h"

double exp(double x)
{
    return __ieee754_exp(x);
}
