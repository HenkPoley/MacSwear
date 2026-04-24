#ifndef _GOA_MATH_H_
#define _GOA_MATH_H_

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.1415926535897932384626433832795
#define M_PI PI
#endif
#endif

#define mdiv(a,b) ( (a)>=0 ? (a)/(b) : ((a)+1)/(b)-1 )
#define mmod(a,b) ( (a)>=0 ? (a)%(b) : ((a)+1)%(b)+(b)-1 )

#ifdef __MINGW32__
#include <math.h>
#define cbrt(x) pow(x,1/3)
#endif

// stolen from Graphics Gems II(?)
int fastsolve(double a,double b,double c,double s[2]);
int fastsolve(double a,double b,double c,double d,double s[3]);
int fastsolve(double a,double b,double c,double d,double e,double s[4]);

int polysolve(double a,double b,double c,double s[2]);
int polysolve(double a,double b,double c,double d,double s[3]);
int polysolve(double a,double b,double c,double d,double e,double s[4]);

#endif
