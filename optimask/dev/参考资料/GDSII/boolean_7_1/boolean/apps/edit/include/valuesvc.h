
#ifndef _VALUES_H_
#define _VALUES_H_

#ifdef _MSWVC_

#include <math.h>
#include <float.h>

#ifndef __cplusplus
#error C++ compiler is required
#endif

#undef	max
#define	max(a,b) ((a) > (b) ? (a) : (b))

#undef	min
#define	min(a,b) ((a) < (b) ? (a) : (b))

#define M_PI		(3.1415926535897932384626433832795028841972)

#define MAXDOUBLE DBL_MAX
#define MAXLONG     0x7fffffff

#else

#if defined(__NetBSD__) || defined(__DragonFly__)

#include <math.h>
#include <limits.h>

#include <float.h>

#ifndef MAXDOUBLE
#define MAXDOUBLE DBL_MAX
#endif
#ifndef MAXLONG
#define MAXLONG LONG_MAX
#endif
#else

#include <values.h>

#endif
#endif


#endif // _VALUES_H_



