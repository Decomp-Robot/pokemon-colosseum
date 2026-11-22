/*
 * Double-precision IEEE arcsine kernel.
 *
 * The endpoints, out-of-domain arguments and tiny arguments are handled
 * directly.  Elsewhere the angle comes from a rational approximation in a
 * reduced variable: x*x near the origin, or (1-|x|)/2 near the endpoints,
 * where a square root supplies the leading term and the low word of pi/2
 * restores the accuracy lost near one.
 */
#include "__fdlibm.h"

#define fabs(x) __fabs(x)

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

extern float quiet_nan[];
extern double sqrt(double);

static const double
pio2_hi =  1.57079632679489655800e+00,
pio2_lo =  6.12323399573676603587e-17,
pS0     =  1.66666666666666657415e-01,
pS1     = -3.25565818622400915405e-01,
pS2     =  2.01212532134862925881e-01,
pS3     = -4.00555345006794114027e-02,
pS4     =  7.91534994289814532176e-04,
pS5     =  3.47933107596021167570e-05,
qS1     = -2.40339491173441421878e+00,
qS2     =  2.02094576023350569471e+00,
qS3     = -6.88283971605453293030e-01,
qS4     =  7.70381505559019352791e-02,
one     =  1.0,
huge    =  1.0e300,
pio4_hi =  7.85398163397448279000e-01;

double __ieee754_asin(double x)
{
	double t,w,p,q,c,r,s;
	int hx,ix;

	hx  = __HI(x);
	ix  = hx&0x7fffffff;
	if(ix>=0x3ff00000) {		/* |x| >= 1 */
	    if(((ix-0x3ff00000)|__LO(x))==0)
		return x*pio2_hi + x*pio2_lo; /* asin(+-1) = +-pi/2 */
	    return quiet_nan[0];	/* asin(|x|>1) is NaN */
	} else if(ix<0x3fe00000) {	/* |x| < 0.5 */
	    if(ix<0x3e400000) {		/* |x| < 2**-27 */
		if(huge+x>one) return x;
	    } else
		t = x*x;
	    p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	    q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	    w = p/q;
	    return x+x*w;
	}
	/* 0.5 <= |x| < 1 */
	w = one-fabs(x);
	t = w*0.5;
	p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	s = sqrt(t);
	if(ix>=0x3FEF3333) {		/* |x| > 0.975 */
	    w = p/q;
	    t = pio2_hi - (2.0*(s+s*w) - pio2_lo);
	} else {
	    w  = s;
	    __LO(w) = 0;
	    c  = (t-w*w)/(s+w);
	    r  = p/q;
	    p  = 2.0*s*r-(pio2_lo-2.0*c);
	    q  = pio4_hi-2.0*w;
	    t  = pio4_hi-(p-q);
	}
	return hx>0 ? t:-t;
}
