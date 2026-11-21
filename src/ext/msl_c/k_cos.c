/*
 * Cosine of a reduced argument x+y, for |x| no larger than pi/4.
 *
 * The polynomial is evaluated in z = x*x, and the low-order part y of the
 * reduction is folded into the final correction.  Near zero the kernel
 * returns one directly, while arguments above 0.3 split off a constant to
 * keep the subtraction of the leading term accurate.
 */

#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

static const double
one = 1.0,
C1 =  4.16666666666666019037e-02,
C2 = -1.38888888888741095749e-03,
C3 =  2.48015872894767294178e-05,
C4 = -2.75573143513906633035e-07,
C5 =  2.08757232129817482790e-09,
C6 = -1.13596475577881948265e-11;

static const double onehalf = 0.5;
static const double split = 0.28125;

double __kernel_cos(double x, double y)
{
	double a, hz, qx, z, r;
	int ix;

	ix = __HI(x)&0x7fffffff;
	if(ix<0x3e400000) {		/* |x| < 2**-27 */
	    if((int)x==0) return one;
	}
	z = x*x;
	r = z*(C1+z*(C2+z*(C3+z*(C4+z*(C5+z*C6)))));
	if(ix < 0x3FD33333) {		/* |x| < 0.3 */
	    return one-(onehalf*z-(z*r-x*y));
	}
	if(ix > 0x3FE90000) {		/* |x| > 0.78125 */
	    qx = split;
	} else {
	    __HI(qx) = ix-0x200000;
	    __LO(qx) = 0;
	}
	hz = onehalf*z-qx;
	a = one-qx;
	return a-(hz-(z*r-x*y));
}
