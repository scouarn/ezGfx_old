#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

#define PI 3.1415926535
#define HALF_PI (0.5 * PI)
#define TWO_PI  (2.0 * PI)
#define QUARTER_PI  (0.75 * PI)


#define MAX(a,b) \
 ({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a > _b ? _a : _b; })

#define MIN(a,b) \
	({ __typeof__ (a) _a = (a); \
		 __typeof__ (b) _b = (b); \
		 _a < _b ? _a : _b; })

#define CLAMP(x, a, b) MAX(a, MIN(b, x));



#define ERROR(m, ...) {printf("[%s] [FATAL ERROR] " m " (in %s::%d)\n", __TIME__, ##__VA_ARGS__, __FILE__, __LINE__); exit(1);}
#define WARNING(m, ...) {printf("[%s] [WARNING] " m " (in %s::%d)\n", __TIME__, ##__VA_ARGS__, __FILE__, __LINE__);}
#define ASSERT(x)  {if (!(x)) ERROR(#x);}
#define ASSERTM(x, m, ...) {if (!(x)) ERROR(#x " : " m, ##__VA_ARGS__);}
#define ASSERTW(x, m, ...) {if (!(x)) WARNING(#x " : " m, ##__VA_ARGS__);}


#endif