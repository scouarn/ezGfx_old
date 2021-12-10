#ifndef _EZGFX_UTILS_H_
#define _EZGFX_UTILS_H_


#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define PI 3.1415926535
#define HALF_PI (PI * 0.5)
#define TWO_PI  (2.0 * PI)
#define QUARTER_PI  (0.25 * PI)


#define MAX(a,b) ({ __typeof__ (a) _a = (a); \
					__typeof__ (b) _b = (b); \
					_a > _b ? _a : _b; })

#define MIN(a,b) ({ __typeof__ (a) _a = (a); \
		 		    __typeof__ (b) _b = (b); \
		 			_a < _b ? _a : _b; })

#define CLAMP(x, a,b) ({ __typeof__ (a) _a = (a); \
		 		    	 __typeof__ (b) _b = (b); \
		 		    	 __typeof__ (x) _x = (x); \
		 				_x < _a ? _a : _x > _b ? _b : _x; })


/*
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
*/


#define SWAP(a, b) ({const __typeof__(a) t = (a); (a) = (b); (b) = t;})

#define LERP(x, a, b) ({ __typeof__ (x) _x = (x); \
				_x*(a) + (1.0 - _x)*(b); })

#define ABS(x) ({ __typeof__ (x) _x = (x); \
				_x < 0 ? -_x : _x; })

#define SIGN(x) ((x) < 0 ? -1 : 1)




#endif /* ezGfx_utils_h */