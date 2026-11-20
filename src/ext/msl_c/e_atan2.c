/*
 * Double-precision arctangent of y/x, with the quadrant taken from the signs
 * of both arguments.
 *
 * Special cases are settled from the operands' full 64-bit patterns: a NaN
 * operand is propagated, x == 1 falls straight through to atan(), and the
 * zero and infinite arguments return the quadrant multiples of pi/2, pi/4
 * and pi directly.  For the ordinary case the exponent difference of y and x
 * decides whether the ratio is safe to form; if |y/x| is very large the angle
 * is pi/2, while a negative x with |y| much smaller leaves z at zero so the
 * final sign dispatch produces an angle near +/-pi.  Otherwise atan() of the
 * magnitude of the ratio is used.  The last step turns the angle in [0,pi/2)
 * into the quadrant selected by m, with pi_lo carrying the low bits of pi.
 */
#include "__fdlibm.h"

#define fabs(x) __fabs(x)

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

#define PI 3.1415926535897931160E+00

typedef enum Atan2Signs {
    ATAN2_Y_POS_X_POS = 0,
    ATAN2_Y_NEG_X_POS = 1,
    ATAN2_Y_POS_X_NEG = 2,
    ATAN2_Y_NEG_X_NEG = 3
} Atan2Signs;

static const double
tiny  = 1.0e-300,
zero  = 0.0,
pi_o_4  = PI / 4.0, /* 0x3FE921FB, 0x54442D18 */
pi_o_2  = PI / 2.0, /* 0x3FF921FB, 0x54442D18 */
pi      = PI,       /* 0x400921FB, 0x54442D18 */
pi_lo   = 1.2246467991473531772E-16; /* 0x3CA1A626, 0x33145C07 */

double __ieee754_atan2(double y, double x)
{
	double z;
	int k,hx,hy,ix,iy;
	Atan2Signs m;
	unsigned lx,ly;

	hx = __HI(x); lx = __LO(x);
	hy = __HI(y); ly = __LO(y);
	ix = hx&0x7fffffff;
	iy = hy&0x7fffffff;
	if(((ix|((lx|-lx)>>31))>0x7ff00000)||
	   ((iy|((ly|-ly)>>31))>0x7ff00000))	/* x or y is NaN */
	   return x+y;
	if(((hx-0x3ff00000)|lx)==0) return atan(y);   /* x=1.0 */
	m = (Atan2Signs)(((hy>>31)&1)|((hx>>30)&2));	/* 2*sign(x)+sign(y) */

    /* when y = 0 */
	if((iy|ly)==0) {
	    switch(m) {
		case ATAN2_Y_POS_X_POS:
		case ATAN2_Y_NEG_X_POS: return y; 	/* atan(+-0,+anything)=+-0 */
		case ATAN2_Y_POS_X_NEG: return  pi+tiny;/* atan(+0,-anything) = pi */
		case ATAN2_Y_NEG_X_NEG: return -pi-tiny;/* atan(-0,-anything) =-pi */
	    }
	}
    /* when x = 0 */
	if((ix|lx)==0) return (hy<0)?  -pi_o_2-tiny: pi_o_2+tiny;

    /* when x is INF */
	if(ix==0x7ff00000) {
	    if(iy==0x7ff00000) {
		switch(m) {
		    case ATAN2_Y_POS_X_POS: return  pi_o_4;		/* atan(+INF,+INF) */
		    case ATAN2_Y_NEG_X_POS: return -pi_o_4;		/* atan(-INF,+INF) */
		    case ATAN2_Y_POS_X_NEG: return  3.0*pi_o_4;		/* atan(+INF,-INF) */
		    case ATAN2_Y_NEG_X_NEG: return -3.0*pi_o_4;		/* atan(-INF,-INF) */
		}
	    } else {
		switch(m) {
		    case ATAN2_Y_POS_X_POS: return  zero;		/* atan(+...,+INF) */
		    case ATAN2_Y_NEG_X_POS: return -zero;		/* atan(-...,+INF) */
		    case ATAN2_Y_POS_X_NEG: return  pi+tiny;		/* atan(+...,-INF) */
		    case ATAN2_Y_NEG_X_NEG: return -pi-tiny;		/* atan(-...,-INF) */
		}
	    }
	}
    /* when y is INF */
	if(iy==0x7ff00000) return (hy<0)? -pi_o_2-tiny: pi_o_2+tiny;

    /* compute y/x */
	k = (iy-ix)>>20;
	if(k > 60) z=pi_o_2+0.5*pi_lo; 	/* |y/x| >  2**60 */
	else if(hx<0&&k<-60) z=0.0; 	/* |y/x| < 2**-60 with x < 0 */
	else z=atan(fabs(y/x));		/* safe to do y/x */
	switch (m) {
	    case ATAN2_Y_POS_X_POS: return       z  ;	/* atan(+,+) */
	    case ATAN2_Y_NEG_X_POS: __HI(z) ^= 0x80000000;
		    return       z  ;	/* atan(-,+) */
	    case ATAN2_Y_POS_X_NEG: return  pi-(z-pi_lo);/* atan(+,-) */
	    default:		    /* ATAN2_Y_NEG_X_NEG */
	    	    return  (z-pi_lo)-pi;/* atan(-,-) */
	}
}
