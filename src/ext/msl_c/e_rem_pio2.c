/*
 * Quadrant reduction for the double-precision trig entry points.
 *
 * Values below pi/4 are returned directly; the next octant uses a
 * first-order pi/2 subtraction.  Larger finite arguments are multiplied by
 * 2/pi and reduced against a multiword pi/2, with the caller supplying the
 * 2/pi fraction table when the kernel is needed.
 */
#include "__fdlibm.h"

#define fabs(x) __fabs(x)

#define __HI(x) (((int *)&(x))[0])
#define __LO(x) (((int *)&(x))[1])

/* Binary64 high word: the biased exponent is an 11-bit field starting at
 * bit 20, and its stored fraction is 23 bits wide (24 significand bits
 * once the implicit leading bit is included). */
#define HI_EXP_SHIFT 20
#define HI_EXP_MASK  0x7ff
#define FRACTION_BITS 23
/* Biased exponent of a value in [2^23,2^24), for 24-bit chunk extraction. */
#define SPLIT_EXP_BIAS (1023 + FRACTION_BITS)

/* Exponent bits lost to cancellation at which the first pi/2 pair
 * (33+53 bits) needs the pio2_2, then the pio2_3, refinement. */
#define PIO2_REFINE_BITS_2 16
#define PIO2_REFINE_BITS_3 49

static const int two_over_pi[] = {
0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,
};

static const int npio2_hw[] = {
0x3ff921fb, 0x400921fb, 0x4012d97c, 0x401921fb, 0x401f6a7a, 0x4022d97c,
0x4025fdbb, 0x402921fb, 0x402c463a, 0x402f6a7a, 0x4031475c, 0x4032d97c,
0x40346b9c, 0x4035fdbb, 0x40378fdb, 0x403921fb, 0x403ab41b, 0x403c463a,
0x403dd85a, 0x403f6a7a, 0x40407e4c, 0x4041475c, 0x4042106c, 0x4042d97c,
0x4043a28c, 0x40446b9c, 0x404534ac, 0x4045fdbb, 0x4046c6cb, 0x40478fdb,
0x404858eb, 0x404921fb,
};

static const double
zero   =  0.0,
half   =  0.5,
two24  =  (double)(1 << 24),
invpio2=  6.36619772367581382433e-01,
pio2_1 =  1.57079632673412561417e+00,
pio2_1t=  6.07710050650619224932e-11,
pio2_2 =  6.07710050630396597660e-11,
pio2_2t=  2.02226624879595063154e-21,
pio2_3 =  2.02226624871116645580e-21,
pio2_3t=  8.47842766036889956997e-32;

int __ieee754_rem_pio2(double x, double *y)
{
	double z,w,t,r,s,fn;
	double tx[3];
	int e0,i,j,nx,n,ix,hx;

	hx = __HI(x);
	ix = hx&0x7fffffff;
	if(ix<=0x3fe921fb) {y[0] = x; y[1] = 0; return 0;}
	if(ix<0x4002d97c) {
	    if(hx>0) {
		z = x - pio2_1;
		if(ix!=0x3ff921fb) {
		    y[0] = z - pio2_1t;
		    y[1] = (z-y[0])-pio2_1t;
		} else {
		    z -= pio2_2;
		    y[0] = z - pio2_2t;
		    y[1] = (z-y[0])-pio2_2t;
		}
		return 1;
	    } else {
		z = x + pio2_1;
		if(ix!=0x3ff921fb) {
		    y[0] = z + pio2_1t;
		    y[1] = (z-y[0])+pio2_1t;
		} else {
		    z += pio2_2;
		    y[0] = z + pio2_2t;
		    y[1] = (z-y[0])+pio2_2t;
		}
		return -1;
	    }
	}
	if(ix<=0x413921fb) {
	    t  = fabs(x);
	    n  = (int) (t*invpio2+half);
	    fn = n;
	    r  = t-fn*pio2_1;
	    w  = fn*pio2_1t;
	    if(n<32&&ix!=npio2_hw[n-1]) {
		y[0] = r-w;
	    } else {
	        j  = ix>>HI_EXP_SHIFT;
	        y[0] = r-w;
	        /* cancellation has eaten the accuracy of the 33+53-bit pi/2 pair */
	        i = j-((__HI(y[0])>>HI_EXP_SHIFT)&HI_EXP_MASK);
	        if(i>PIO2_REFINE_BITS_2) {
		    s  = r;
		    w  = fn*pio2_2;
		    r  = s-w;
		    w  = fn*pio2_2t-((s-r)-w);
		    y[0] = r-w;
		    /* still short of full precision: use the third-order tail */
		    i = j-((__HI(y[0])>>HI_EXP_SHIFT)&HI_EXP_MASK);
		    if(i>PIO2_REFINE_BITS_3)  {
		    	t  = r;
		    	w  = fn*pio2_3;
		    	r  = t-w;
		    	w  = fn*pio2_3t-((t-r)-w);
		    	y[0] = r-w;
		    }
		}
	    }
	    y[1] = (r-y[0])-w;
	    if(hx<0) 	{y[0] = -y[0]; y[1] = -y[1]; return -n;}
	    else	 return n;
	}
	if(ix>=0x7ff00000) {
	    y[0]=y[1]=x-x; return 0;
	}
	__LO(z) = __LO(x);
	/* e0 is ilogb(|x|)-23; rescale z into [2^23,2^24) so the two chunks
	 * extracted below each hold 24 significant bits. */
	e0 	= (ix>>HI_EXP_SHIFT)-SPLIT_EXP_BIAS;
	__HI(z) = ix - (e0<<HI_EXP_SHIFT);
	for(i=0;i<2;i++) {
		tx[i] = (int)z;
		z     = (z-tx[i])*two24;
	}
	tx[2] = z;
	nx = 3;
	while(tx[nx-1]==zero) nx--;
	n  =  __kernel_rem_pio2(tx,y,e0,nx,REM_PIO2_PREC_DOUBLE,two_over_pi);
	if(hx<0) {y[0] = -y[0]; y[1] = -y[1]; return -n;}
	return n;
}
