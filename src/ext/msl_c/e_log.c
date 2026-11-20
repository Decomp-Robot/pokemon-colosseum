/*
 * Double-precision natural logarithm kernel.
 *
 * Subnormal arguments are first scaled by 2^54.  The binary exponent is then
 * split off and its ln(2) contribution is kept in high and low pieces, while
 * the mantissa is normalized near one and reduced with s = f/(2+f).  The
 * remaining logarithm comes from even and odd polynomials in s*s.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

extern int errno;

#define EDOM 33

/* Binary64 high word: the biased exponent sits above the 20-bit fraction
 * field and is unbiased by 1023. */
#define EXPONENT_SHIFT 20
#define EXPONENT_BIAS 1023
#define MANTISSA_MASK 0x000fffff

/* Adding this carry before testing its bit normalizes at the sqrt(2) boundary;
 * the same carry is reused to adjust the exponent. */
#define NORMALIZE_CARRY 0x100000
#define NORMALIZE_CUTOFF 0x6a09c

/* Mantissa interval selecting the half-square evaluation form. */
#define HALF_FORM_LOW 0x6147a
#define HALF_FORM_HIGH 0x6b851

static const double
ln2_hi  =  6.93147180369123816490e-01,	/* 3fe62e42 fee00000 */
ln2_lo  =  1.90821492927058770002e-10,	/* 3dea39ef 35793c76 */
two54   =  18014398509481984.0,  /* 43500000 00000000 */
Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

static double zero   =  0.0;

double __ieee754_log(double x)
{
	double hfsq,f,s,z,R,w,t1,t2,dk;
	int k,hx,i,j;
	unsigned lx;

	hx = __HI(x);
	lx = __LO(x);

	k=0;
	if (hx < 0x00100000) {			/* x < 2**-1022  */
	    if (((hx&0x7fffffff)|lx)==0)
		return -two54/zero;		/* log(+-0)=-inf */
	    if (hx<0) {errno=EDOM; return (x-x)/zero;}	/* log(-#) = NaN */
	    k -= 54; x *= two54; /* subnormal number, scale up x */
	    hx = __HI(x);
	}
	if (hx >= 0x7ff00000) return x+x;
	k += (hx>>EXPONENT_SHIFT)-EXPONENT_BIAS;
	hx &= MANTISSA_MASK;
	i = (hx+NORMALIZE_CARRY-NORMALIZE_CUTOFF)&NORMALIZE_CARRY;
	__HI(x) = hx|(i^0x3ff00000);	/* normalize x or x/2 */
	k += (i>>EXPONENT_SHIFT);
	f = x-1.0;
	if((MANTISSA_MASK&(2+hx))<3) {	/* |f| < 2**-20 */
	    if(f==zero) {if(k==0) return zero;  else {dk=k;
				 return dk*ln2_hi+dk*ln2_lo;}
	    }
	    R = f*f*(0.5-(1.0/3.0)*f);
	    if(k==0) return f-R; else {dk=k;
	    	     return dk*ln2_hi-((R-dk*ln2_lo)-f);}
	}
	s = f/(2.0+f);
	dk = k;
	z = s*s;
	i = hx-HALF_FORM_LOW;
	w = z*z;
	j = HALF_FORM_HIGH-hx;
	t1= w*(Lg2+w*(Lg4+w*Lg6));
	t2= z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7)));
	R = t2+t1;
	i |= j;
	if(i>0) {
	    hfsq=0.5*f*f;
	    if(k==0) return f-(hfsq-s*(hfsq+R)); else
		     return dk*ln2_hi-((hfsq-(s*(hfsq+R)+dk*ln2_lo))-f);
	} else {
	    if(k==0) return f-s*(f-R); else
		     return dk*ln2_hi-((s*(f-R)-dk*ln2_lo)-f);
	}
}
