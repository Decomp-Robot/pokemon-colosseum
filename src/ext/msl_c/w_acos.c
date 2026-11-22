/*
 * Public double-precision acos entry point.  The kernel inspects the operand
 * and selects the endpoint, small-argument or reduced-angle path; this member
 * only forwards the call.
 */
#include "__fdlibm.h"

double acos(double x)
{
    return __ieee754_acos(x);
}
