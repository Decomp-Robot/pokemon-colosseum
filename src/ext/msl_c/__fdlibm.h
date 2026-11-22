#ifndef _MSL_C_FDLIBM_INTERNAL_H_
#define _MSL_C_FDLIBM_INTERNAL_H_

/* Number of doubles the caller expects in the reduced remainder, and the
 * matching working precision used by the large-argument reducer. */
typedef enum RemPio2Precision {
    REM_PIO2_PREC_SINGLE = 0,      /* one double, in y[0] */
    REM_PIO2_PREC_DOUBLE_LOW = 1,  /* two doubles, shorter pi/2 reconstruction */
    REM_PIO2_PREC_DOUBLE = 2,      /* two doubles */
    REM_PIO2_PREC_TRIPLE = 3       /* three doubles, in y[0..2] */
} RemPio2Precision;

double atan(double);

double copysign(double,double);

double __ieee754_acos(double);
double __ieee754_asin(double);
double __ieee754_atan2(double,double);
double __ieee754_exp(double);
double __ieee754_fmod(double,double);
double __ieee754_log(double);
double __ieee754_pow(double,double);
int __ieee754_rem_pio2(double,double *);

double __kernel_cos(double,double);
int __kernel_rem_pio2(double *x,double *y,int e0,int nx,RemPio2Precision prec,const int *ipio2);
double __kernel_sin(double,double,int);
double __kernel_tan(double,double,int);

#endif /* _MSL_C_FDLIBM_INTERNAL_H_ */
