#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>




#define LIST_OF(T) struct __list_of_##T {T hd; struct __list_of_##T *tl;}


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


#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))


#define SWAP(a, b) ({const __typeof__(a) t = (a); (a) = (b); (b) = t;})
#define LERP(x, a, b) ((x)*(a) + (1.0 - (x))*(b))
#define ABS(x) (((x) < 0) ? -(x) : (x))


#define ERROR(m, ...) 	{fprintf(stderr, "[%s] [FATAL ERROR] " m " (in %s::%d)\n", __TIME__, ##__VA_ARGS__, __FILE__, __LINE__); exit(1);}
#define WARNING(m, ...) {fprintf(stderr, "[%s] [WARNING]   	 " m " (in %s::%d)\n", __TIME__, ##__VA_ARGS__, __FILE__, __LINE__);}
#define ASSERT(x)  {if (!(x)) ERROR(#x);}
#define ASSERTM(x, m, ...) {if (!(x)) ERROR(#x " : " m, ##__VA_ARGS__);}
#define ASSERTW(x, m, ...) {if (!(x)) WARNING(#x " : " m, ##__VA_ARGS__);}



#endif