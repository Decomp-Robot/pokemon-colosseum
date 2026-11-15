#ifndef _MSL_MATH_H
#define _MSL_MATH_H

/* Metrowerks Standard Library <math.h> for the MusyX SDK sources. The library
 * provides the double-precision entry points and the hardware single-precision
 * tanf()/sinf()/cosf() family; the other single-precision names are inline
 * wrappers over their double-precision counterparts. */

double atan(double x);
double atan2(double y, double x);
double ceil(double x);
double cos(double x);
double exp(double x);
double floor(double x);
double frexp(double x, int *exponent);
double ldexp(double x, int exponent);
double log(double x);
double log10(double x);
double modf(double x, double *intpart);
double pow(double base, double exponent);
double sin(double x);
double sqrt(double x);
double tan(double x);
double fmod(double x, double y);

float tanf(float x);
float sinf(float x);
float cosf(float x);

static inline float fmodf(float x, float y) { return (float)fmod(x, y); }

#endif /* _MSL_MATH_H */
