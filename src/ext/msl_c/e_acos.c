/*
 * Double-precision IEEE arccosine kernel.
 *
 * The endpoint and out-of-domain cases are settled from the high and low
 * words of the argument.  For |x| below one half, the angle is pi/2 minus a
 * small correction computed from a rational approximation in x*x.  Above
 * one half the angle is reduced through a square root, with the low half of
 * the root split off before the final reconstruction so that the result
 * keeps its accuracy near the endpoints.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

extern double sqrt(double);

static const double
pi      =  3.14159265358979311600e+00,
pio2_hi =  1.57079632679489655800e+00,
pio2_lo =  6.12323399573676603587e-17,
pS0     =  1.66666666666666657415e-01,
pS1     = -3.25565818622400915405e-01,
pS2     =  2.01212532134862925881e-01,
pS3     = -4.00555345006794114027e-02,
pS4     =  7.91534994289814532176e-04,
pS5     =  3.47933107596021167570e-05,
one     =  1.0,
qS1     = -2.40339491173441421878e+00,
qS2     =  2.02094576023350569471e+00,
qS3     = -6.88283971605453293030e-01,
qS4     =  7.70381505559019352791e-02;

double __ieee754_acos(double x)
{
	double z,p,q,r,w,s,c,df;
	int hx,ix;

	hx  = __HI(x);
	ix  = hx&0x7fffffff;
	if(ix>=0x3ff00000) {		/* |x| >= 1 */
	    if(((ix-0x3ff00000)|__LO(x))==0) {
		if(hx>0) return 0.0;	/* acos(1) = 0 */
		else return pi+2.0*pio2_lo;	/* acos(-1) = pi */
	    }
	    return quiet_nan[0];	/* acos(|x|>1) is NaN */
	}
	if(ix<0x3fe00000) {		/* |x| < 0.5 */
	    if(ix<=0x3c600000) return pio2_hi+pio2_lo; /* |x| < 2**-57 */
	    z = x*x;
	    p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	    q = one+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	    r = p/q;
	    return pio2_hi - (x - (pio2_lo - x*r));
	} else if(hx<0) {		/* x < -0.5 */
	    z = (one+x)*0.5;
	    s = sqrt(z);
	    p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	    q = one+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	    r = p/q;
	    w = r*s-pio2_lo;
	    return pi - 2.0*(s+w);
	} else {			/* x > 0.5 */
	    z = (one-x)*0.5;
	    s = sqrt(z);
	    df = s;
	    __LO(df) = 0;
	    c = (z-df*df)/(s+df);
	    p = z*(pS0+z*(pS1+z*(pS2+z*(pS3+z*(pS4+z*pS5)))));
	    q = one+z*(qS1+z*(qS2+z*(qS3+z*qS4)));
	    r = p/q;
	    w = r*s+c;
	    return 2.0*(df+w);
	}
}
