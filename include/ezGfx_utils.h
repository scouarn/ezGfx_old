#ifndef _EZGFX_UTILS_H_
#define _EZGFX_UTILS_H_


#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define PI 3.1415926535
#define HALF_PI (PI * 0.5)
#define TWO_PI  (2.0 * PI)
#define QUARTER_PI  (0.25 * PI)

/*
#define MAX(a,b) ({ __typeof__ (a) _a = (a); \
					__typeof__ (b) _b = (b); \
					_a > _b ? _a : _b; })

#define MIN(a,b) ({ __typeof__ (a) _a = (a); \
		 		    __typeof__ (b) _b = (b); \
		 			_a < _b ? _a : _b; })
*/

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))


#define SWAP(a, b) ({const __typeof__(a) t = (a); (a) = (b); (b) = t;})
#define LERP(x, a, b) ((x)*(a) + (1.0 - (x))*(b))
#define ABS(x) (((x) < 0) ? -(x) : (x))






/* from M_datastructures :
 * EZ_throw
 * EZ_assert
 * EZ_error
 * EZ_warning				 */


#ifdef EZ_DEBUG

#define EZ_throw(type, message) {\
	char buffer[16];\
	time_t sec = time(NULL);\
	struct tm* tm_info = localtime(&sec);\
	strftime(buffer, 16, "%H:%M:%S", tm_info);\
	fprintf(stderr, "[%s - %s - %s] %s : %s\n", buffer, __FILE__, __func__, type, message);\
}\

#define EZ_assert(test, message) {\
	if (!(test)) {\
		EZ_throw("FAILED TO ASSERT " #test, message);\
		exit(EXIT_FAILURE);\
	}\
}\

#else

#define EZ_throw(type, message) {\
	char buffer[16];\
	time_t sec = time(NULL);\
	struct tm* tm_info = localtime(&sec);\
	strftime(buffer, 16, "%H:%M:%S", tm_info);\
	fprintf(stderr, "[%s] %s : %s\n", buffer, type, message);\
}\

#define EZ_assert(test, message) {\
	if (!(test)) {\
		EZ_error(message);\
	}\
}\

#endif


#define EZ_error(message) {\
	EZ_throw("ERROR", message);\
	exit(EXIT_FAILURE);\
}\

#define EZ_warning(message) {\
	EZ_throw("WARNING", message);\
}\





#endif /* ezGfx_utils_h */