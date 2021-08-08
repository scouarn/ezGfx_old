#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;
typedef long double f80;


#define MAX(a,b) \
 ({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a > _b ? _a : _b; })

#define MIN(a,b) \
	({ __typeof__ (a) _a = (a); \
		 __typeof__ (b) _b = (b); \
		 _a < _b ? _a : _b; })


#ifdef __DEBUG__

	#define ERROR(m)
	#define WARNING(m)
	#define ASSERT(x)

#endif



#endif