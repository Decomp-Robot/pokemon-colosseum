/*
 * Double-precision remainder kernel.
 *
 * After removing the exceptional cases, the dividend and divisor are reduced
 * to signed 53-bit fractions with independent binary exponents.  The common
 * exponent range is walked with inline 32-bit shift/subtract pairs so that no
 * floating division is needed; the remaining fraction is then reassembled as
 * a double carrying the dividend's sign.
 */
#include "__fdlibm.h"

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

/* Signed zero of the dividend, indexed by its sign bit. */
static const double Zero[] = {0.0, -0.0,};

double __ieee754_fmod(double x, double y)
{
	int n,hx,hy,hz,ix,iy,sx,i;
	unsigned int lx,ly,lz;

	hx = __HI(x); lx = __LO(x);
	hy = __HI(y); ly = __LO(y);
	sx = hx&0x80000000;
	hx ^= sx;
	hy &= 0x7fffffff;

    /* purge off exception values */
	if((hy|ly)==0||(hx>=0x7ff00000)||
	   ((hy|((ly|(-ly))>>31))>0x7ff00000))
	    return (x*y)/(x*y);
	if(hx<=hy) {
	    if((hx<hy)||(lx<ly)) return x;	/* |x|<|y| return x */
	    if(lx==ly) return Zero[(unsigned int)sx>>31];	/* |x|=|y| return x*0*/
	}

    /* determine ix = ilogb(x) */
	if(hx<0x00100000) {	/* subnormal x */
	    if(hx==0) {
		for (ix = -1043, i=lx; i>0; i<<=1) ix -=1;
	    } else {
		for (ix = -1022,i=hx<<11; i>0; i<<=1) ix -=1;
	    }
	} else ix = (hx>>20)-1023;

    /* determine iy = ilogb(y) */
	if(hy<0x00100000) {	/* subnormal y */
	    if(hy==0) {
		for (iy = -1043, i=ly; i>0; i<<=1) iy -=1;
	    } else {
		for (iy = -1022,i=hy<<11; i>0; i<<=1) iy -=1;
	    }
	} else iy = (hy>>20)-1023;

    /* set up {hx,lx}, {hy,ly} and align y to x */
	if(ix >= -1022)
	    hx = 0x00100000|(0x000fffff&hx);
	else {		/* subnormal x, shift x to normal */
	    n = -1022-ix;
	    if(n<=31) {
	        hx = (hx<<n)|(lx>>(32-n));
	        lx <<= n;
	    } else {
	        hx = lx<<(n-32);
	        lx = 0;
	    }
	}
	if(iy >= -1022)
	    hy = 0x00100000|(0x000fffff&hy);
	else {		/* subnormal y, shift y to normal */
	    n = -1022-iy;
	    if(n<=31) {
	        hy = (hy<<n)|(ly>>(32-n));
	        ly <<= n;
	    } else {
	        hy = ly<<(n-32);
	        ly = 0;
	    }
	}

    /* fix point fmod */
	n = ix - iy;
	while(n--) {
	    hz=hx-hy;lz=lx-ly;
	    if(lx<ly) hz -= 1;
	    if(hz<0){hx = hx+hx+(lx>>31); lx = lx+lx;}
	    else {
	        if((hz|lz)==0) return Zero[(unsigned int)sx>>31]; /* return sign(x)*0 */
		hx = hz+hz+(lz>>31); lx = lz+lz;
	    }
	}
	hz=hx-hy; lz=lx-ly; if(lx<ly) hz -= 1;
	if(hz>=0) {hx=hz;lx=lz;}

    /* convert back to floating value and restore the sign */
	if((hx|lx)==0) return Zero[(unsigned int)sx>>31];	/* return sign(x)*0 */
	while(hx<0x00100000) {		/* normalize x */
	    hx = hx+hx+(lx>>31); lx = lx+lx;
	    iy -= 1;
	}
	if(iy>=-1022) {
	    hx = (hx-0x00100000)|((iy+1023)<<20);
	    __LO(x) = lx;
	    __HI(x) = hx|sx;
	}
	else { /* subnormal output */
	    n = -1022-iy;
	    if(n<=20) {
		lx = (lx>>n)|((unsigned int)hx<<(32-n));
		hx >>= n;
	    } else if (n<=31) {
		lx = ((unsigned int)hx<<(32-n))|(lx>>n); hx = sx;
	    } else {
		lx = hx>>(n-32); hx = sx;
	    }
	    __LO(x) = lx;
	    __HI(x) = hx|sx;
	}
	return x;
}
