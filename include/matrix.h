

#ifndef EZ_MATRIX 
#define EZ_MATRIX

#include <math.h>


typedef struct {int x, y;} vec2i;
typedef struct {int x, y, z;} vec3i;
typedef struct {int x, y, z, w;} vec4i;

typedef struct {float x, y;} vec2f;
typedef struct {float x, y, z;} vec3f;
typedef struct {float x, y, z, w;} vec4f;



typedef struct {float a0, a1, 
					  b0, b1;
				} mat2x2;

typedef struct {float a0, a1, a2, 
					  b0, b1, b2, 
					  c0, c1, c2;
				} mat3x3;

typedef struct {float a0, a1, a2, a3, 
					  b0, b1, b2, b3, 
					  c0, c1, c2, c3,
					  d0, d1, d2, d3;
				} mat4x4;



#define VADD2(u, v) ((__typeof__(u)){(u).x + (v).x, (u).y + (v).y})
#define VADD3(u, v) ((__typeof__(u)){(u).x + (v).x, (u).y + (v).y, (u).z + (v).z})
#define VADD4(u, v) ((__typeof__(u)){(u).x + (v).x, (u).y + (v).y, (u).z + (v).z, (u).v + (v).v})

#define VSUB2(u, v)((__typeof__(u)){(u).x - (v).x, (u).y - (v).y})
#define VSUB3(u, v) ((__typeof__(u)){(u).x - (v).x, (u).y - (v).y, (u).z - (v).z})
#define VSUB4(u, v) ((__typeof__(u)){(u).x - (v).x, (u).y - (v).y, (u).z - (v).z, (u).v - (v).v})

#define VMUL2(u, a) ((__typeof__(u)){(u).x * a, (u).y * a})
#define VMUL3(u, a) ((__typeof__(u)){(u).x * a, (u).y * a, (u).z * a})
#define VMUL4(u, a) ((__typeof__(u)){(u).x * a, (u).y * a, (u).z * a, (u).v * a})

#define VDOT2(u, v) ((vec2f) {u.x * v.x,  u.y * v.y})
#define VDOT3(u, v) ((vec3f) {u.x * v.x,  u.y * v.y,	 u.z * v.z})
#define VDOT4(u, v) ((vec4f) {u.x * v.x,  u.y * v.y,	 u.z * v.z,  u.w * v.w})

#define NUL2i ((vec2i){0, 0})
#define NUL3i ((vec3i){0, 0, 0})
#define NUL4i ((vec4i){0, 0, 0, 0})

#define NUL2f ((vec2f){0.0f, 0.0f})
#define NUL3f ((vec3f){0.0f, 0.0f, 0.0f})
#define NUL4f ((vec4f){0.0f, 0.0f, 0.0f, 0.0f})





#define MADD2x2(a, b) ((mat2x2){(a).a0 + (b).a0, (a).a1 + (b).a1,	\
							    (a).b0 + (b).b0, (a).b1 + (b).b1})

#define MADD3x3(a, b) ((mat3x3){(a).a0 + (b).a0, (a).a1 + (b).a1, (a).a2 + (b).a2,	\
							    (a).b0 + (b).b0, (a).b1 + (b).b1, (a).b2 + (b).b2,	\
							    (a).c0 + (b).c0, (a).c1 + (b).c1, (a).c2 + (b).c2})

#define MADD4x4(a, b) ((mat4x4){(a).a0 + (b).a0, (a).a1 + (b).a1, (a).a2 + (b).a2, (a).a3 + (b).a3,	\
							    (a).b0 + (b).b0, (a).b1 + (b).b1, (a).b2 + (b).b2, (a).b3 + (b).b3,	\
							    (a).c0 + (b).c0, (a).c1 + (b).c1, (a).c2 + (b).c2, (a).c3 + (b).c3,	\
							    (a).d0 + (b).d0, (a).d1 + (b).d1, (a).d2 + (b).d2, (a).d3 + (b).d3})


#define MSCA2x2(a, s) ((mat2x2){(a).a0 * c, (a).a1 * c,	\
							    (a).b0 * c, (a).b1 * c})

#define MSCA3x3(a, s) ((mat3x3){(a).a0 * c, (a).a1 * c, (a).a2 * c,	\
							    (a).b0 * c, (a).b1 * c, (a).b2 * c,	\
							    (a).c0 * c, (a).c1 * c, (a).c2 * c})

#define MSCA4x4(a, s) ((mat4x4){(a).a0 * c, (a).a1 * c, (a).a2 * c, (a).a3 * c,	\
							    (a).b0 * c, (a).b1 * c, (a).b2 * c, (a).b3 * c,	\
							    (a).c0 * c, (a).c1 * c, (a).c2 * c, (a).c3 * c,	\
							    (a).d0 * c, (a).d1 * c, (a).d2 * c, (a).d3 * c})


#define MMUL2x2(a, b) ((mat2x2){(a).a0*(b).a0 + (a).a1*(b).b0, 	(a).a0*(b).a1 + (a).a1*(b).b1,	\
							    (a).b0*(b).a0 + (a).b1*(b).b0, 	(a).b0*(b).a1 + (a).b1*(b).b1})

#define MMUL3x3(a, b) ((mat3x3){(a).a0*(b).a0 + (a).a1*(b).b0 + (a).a2*(b).c0,	(a).a0*(b).a1 + (a).a1*(b).b1 + (a).a2*(b).c1,	(a).a0*(b).a2 + (a).a1*(b).b2 + (a).a2*(b).c2,	\
							    (a).b0*(b).a0 + (a).b1*(b).b0 + (a).b2*(b).c0,	(a).b0*(b).a1 + (a).b1*(b).b1 + (a).b2*(b).c1,	(a).b0*(b).a2 + (a).b1*(b).b2 + (a).b2*(b).c2,	\
							    (a).c0*(b).a0 + (a).c1*(b).b0 + (a).c2*(b).c0,	(a).c0*(b).a1 + (a).c1*(b).b1 + (a).c2*(b).c1,	(a).c0*(b).a2 + (a).c1*(b).b2 + (a).c2*(b).c2})

#define MMUL4x4(a, b) ((mat4x4){(a).a0*(b).a0 + (a).a1*(b).b0 + (a).a2*(b).c0 + (a).a3*(b).d0,	(a).a0*(b).a1 + (a).a1*(b).b1 + (a).a2*(b).c1 + (a).a3*(b).d1,	(a).a0*(b).a2 + (a).a1*(b).b2 + (a).a2*(b).c2 + (a).a3*(b).d2,	(a).a0*(b).a3 + (a).a1*(b).b3 + (a).a2*(b).c3 + (a).a3*(b).d3,	\
							    (a).b0*(b).a0 + (a).b1*(b).b0 + (a).b2*(b).c0 + (a).b3*(b).d0,	(a).b0*(b).a1 + (a).b1*(b).b1 + (a).b2*(b).c1 + (a).b3*(b).d1,	(a).b0*(b).a2 + (a).b1*(b).b2 + (a).b2*(b).c2 + (a).b3*(b).d2,	(a).b0*(b).a3 + (a).b1*(b).b3 + (a).b2*(b).c3 + (a).b3*(b).d3,	\
							    (a).c0*(b).a0 + (a).c1*(b).b0 + (a).c2*(b).c0 + (a).c3*(b).d0,	(a).c0*(b).a1 + (a).c1*(b).b1 + (a).c2*(b).c1 + (a).c3*(b).d1,	(a).c0*(b).a2 + (a).c1*(b).b2 + (a).c2*(b).c2 + (a).c3*(b).d2,	(a).c0*(b).a3 + (a).c1*(b).b3 + (a).c2*(b).c3 + (a).c3*(b).d3,	\
							    (a).d0*(b).a0 + (a).d1*(b).b0 + (a).d2*(b).c0 + (a).d3*(b).d0,	(a).d0*(b).a1 + (a).d1*(b).b1 + (a).d2*(b).c1 + (a).d3*(b).d1,	(a).d0*(b).a2 + (a).d1*(b).b2 + (a).d2*(b).c2 + (a).d3*(b).d2,	(a).d0*(b).a3 + (a).d1*(b).b3 + (a).d2*(b).c3 + (a).d3*(b).d3})


#define MMUL2V2(a, b) ((vec2f){(a).a0*(v).x + (a).a1*(v).y,	\
							   (a).b0*(v).x + (a).b1*(v).y})

#define MMUL2V1(a, b) ({(a).a0*(v).x + (a).a1})


#define MMUL3V3(a, b) ((vec3f){(a).a0*(v).x + (a).a1*(v).y + (a).a2*(v).z,	\
							   (a).b0*(v).x + (a).b1*(v).y + (a).b2*(v).z,	\
							   (a).c0*(v).x + (a).c1*(v).y + (a).c2*(v).z})

#define MMUL3V2(a, b) ((vec3f){(a).a0*(v).x + (a).a1*(v).y + (a).a2*(v),	\
							   (a).b0*(v).x + (a).b1*(v).y + (a).b2*(v)})


#define MMUL4V4(a, b) ((vec4f){(a).a0*(v).x + (a).a1*(v).y + (a).a2*(v).z + (a).a3*(v).w,	\
							   (a).b0*(v).x + (a).b1*(v).y + (a).b2*(v).z + (a).b3*(v).w,	\
							   (a).c0*(v).x + (a).c1*(v).y + (a).c2*(v).z + (a).c3*(v).w,	\
							   (a).d0*(v).x + (a).d1*(v).y + (a).d2*(v).z + (a).d3*(v).w})

#define MMUL4V3(a, b) ((vec4f){(a).a0*(v).x + (a).a1*(v).y + (a).a2*(v).z + (a).a3,	\
							   (a).b0*(v).x + (a).b1*(v).y + (a).b2*(v).z + (a).b3,	\
							   (a).c0*(v).x + (a).c1*(v).y + (a).c2*(v).z + (a).c3})


#define MDET2x2(a) ((a).a0*(a).b1	\
				   -(a).a1*(a).b0)

#define MDET3x3(a) ((a).a0*((a).b1*(a).c2	-	(a).b2*(a).c1)	\
				   -(a).a1*((a).b0*(a).c2	-	(a).b2*(a).c1)	\
				   +(a).a2*((a).b0*(a).c1	-	(a).b1*(a).c1))

#define MDET4x4(a)


#define MIVD2x2(a, d) ((mat2x2){(a).b1/d, -(a).a1/d, 	\
							   -(a).b0/d,  (a).a0/d})

#define MIVD3x3(a, d) ((mat3x3){((a).c2*(a).b1 - (a).c1*(a).b2)/d,	-((a).c2*(a).a1 - (a).c1*(a).a2)/d,  ((a).b2*(a).b1 - (a).b1*(a).b2)/d,		\
							   -((a).c2*(a).b0 - (a).c0*(a).b2)/d,	 ((a).c2*(a).a0 - (a).c0*(a).a2)/d, -((a).b2*(a).b0 - (a).b0*(a).b2)/d,		\
							    ((a).c1*(a).b0 - (a).c0*(a).b1)/d,	-((a).c1*(a).a0 - (a).c0*(a).a1)/d,  ((a).b1*(a).b0 - (a).b0*(a).b1)/d})


#define MIVD4x4(a, d) ((mat4x4){})

#define MINV2x2(a) MIVD2x2(a, MDET2x2(a))
#define MINV3x3(a) MIVD3x3(a, MDET3x3(a))
#define MINV4x4(a) MIVD4x4(a, MDET4x4(a))




#define ID2x2 ((mat2x2){1.0f, 0.0f, 0.0f, 1.0f})
#define ID3x3 ((mat3x3){1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f})
#define ID4x4 ((mat4x4){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f})

#define AFFAPP(a, b) MMUL3x3(a, b)
#define AFFFWD(a, v) MMUL3V2(a, v)
#define AFFIDT() ID3x3
#define AFFROT(u) ((mat3x3){cosf(u), -sinf(u), 0.0f, sinf(u), cos(u), 0.0f, 0.0f, 0.0f, 1.0f})
#define AFFSCA(u) ((mat3x3){u.x, 0.0f, 0.0f, 0.0f, u.y, 0.0f, 0.0f, 0.0f, 1.0f})
#define AFFSHR(u) ((mat3x3){1.0f, u.x, 0.0f, u.y, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f})
#define AFFTNS(u) ((mat3x3){1.0f, 0.0f, u.x, 0.0f, 1.0f, u.y, 0.0f, 0.0f, 1.0f})


#endif