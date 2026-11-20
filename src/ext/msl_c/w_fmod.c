/*
 * Public double-precision remainder entry.  The kernel handles every special
 * case, so this layer only preserves the caller's link and the two-double ABI.
 */

#include "__fdlibm.h"

double fmod(double x, double y)
{
    return __ieee754_fmod(x, y);
}
