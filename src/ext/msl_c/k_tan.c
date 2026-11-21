/*
 * Tangent of a reduced argument x+y, for |x| no larger than pi/4.
 *
 * The polynomial is evaluated in z = x*x; iy selects between the result
 * itself (iy = 1) and its reciprocal (iy = -1), the latter being refined
 * from chopped high halves so it loses only one further rounding.
 * Arguments above about 0.674 answer through the complementary formula
 * 1 - 2w/(w + iy) around pi/4 instead, with the low part y folded into
 * the reduced argument beforehand.
 */

#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

static const double
one = 1.0,
pio4  =  7.8539816339744827900e-01,  /* 0x3FE921FB, 0x54442D18 */
pio4lo = 3.0616169978683830179e-17,  /* 0x3C81A626, 0x33145C07 */
T[13] =  {
    3.3333333333333409199e-01,  /* 0x3FD55555, 0x55555563 */
    1.3333333333320124270e-01,  /* 0x3FC11111, 0x1110FE7A */
    5.3968253976226052138e-02,  /* 0x3FABA1BA, 0x1BB341FE */
    2.1869488294859542460e-02,  /* 0x3F9664F4, 0x8406D637 */
    8.8632398235993000574e-03,  /* 0x3F8226E3, 0xE96E8493 */
    3.5920791075913123536e-03,  /* 0x3F6D6D22, 0xC9560328 */
    1.4562094543252902552e-03,  /* 0x3F57DBC8, 0xFEE08315 */
    5.8804124082026409687e-04,  /* 0x3F4344D8, 0xF2F26501 */
    2.4646313481846990681e-04,  /* 0x3F3026F7, 0x1A8D1068 */
    7.8179444293955709230e-05,  /* 0x3F147E88, 0xA03792A6 */
    7.1407249138260819031e-05,  /* 0x3F12B80F, 0x32F0A7E9 */
   -1.8558637485527545665e-05,  /* 0xBEF375CB, 0xDB605373 */
    2.5907305186363371288e-05,  /* 0x3EFB2A70, 0x74BF7AD4 */
};

double __kernel_tan(double x, double y, int iy)
{
	double z, r, v, w, s, u;
	int ix, hx;

	hx = __HI(x);
	ix = hx&0x7fffffff;
	if(ix<0x3e300000) {		/* |x| < 2**-28 */
	    if((int)x==0) {		/* generate inexact */
		if((ix|__LO(x)|(iy+1))==0) {
		    u = __fabs(x);
		    return one/u;
		}
		else
		    return (iy==1)? x: -one/x;
	    }
	}
	if(ix>=0x3FE59428) {		/* |x| >= 0.6744 */
	    if(hx<0) {
		x = -x;
		y = -y;
	    }
	    z = pio4-x;
	    w = pio4lo-y;
	    x = z+w;
	    y = 0.0;
	}
	z = x*x;
	w = z*z;
	/* break x^5*(T[1]+x^2*T[2]+...) into
	 *    x^5*(T[1]+x^4*T[3]+...+x^20*T[11]) +
	 *    x^5*(x^2*(T[2]+x^4*T[4]+...+x^20*T[12]))
	 */
	r = T[1]+w*(T[3]+w*(T[5]+w*(T[7]+w*(T[9]+w*T[11]))));
	v = z*(T[2]+w*(T[4]+w*(T[6]+w*(T[8]+w*(T[10]+w*T[12])))));
	s = z*x;
	r = y+z*(s*(r+v)+y);
	r += T[0]*s;
	w = x+r;
	if(ix>=0x3FE59428) {		/* combine around pi/4 */
	    v = iy;
	    return (1-((hx>>30)&2))*(v-2.0*(x-(w*w/(w+v)-r)));
	}
	if(iy==1)
	    return w;
	else {
	    /* -1/w, refined from the chopped high halves of w and of the
	     * estimate so it loses only one further rounding */
	    double a, t;
	    z = w;
	    __LO(z) = 0;
	    v = r-(z-x);
	    t = a = -1.0/w;
	    __LO(t) = 0;
	    s = 1.0+t*z;
	    return t+a*(s+t*v);
	}
}
