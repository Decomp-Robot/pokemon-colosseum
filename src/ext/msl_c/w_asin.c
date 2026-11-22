/*
 * Public double-precision asin entry point.  The kernel handles the sign and
 * the endpoint and reduction paths; this member only forwards the call.
 */
#include "__fdlibm.h"

double asin(double x)
{
    return __ieee754_asin(x);
}
