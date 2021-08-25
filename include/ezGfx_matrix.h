

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




#define NUL2i ((vec2i){0, 0})
#define NUL3i ((vec3i){0, 0, 0})
#define NUL4i ((vec4i){0, 0, 0, 0})

#define NUL2f ((vec2f){0.0f, 0.0f})
#define NUL3f ((vec3f){0.0f, 0.0f, 0.0f})
#define NUL4f ((vec4f){0.0f, 0.0f, 0.0f, 0.0f})

#define ID2x2 ((mat2x2){1.0f, 0.0f, 0.0f, 1.0f})
#define ID3x3 ((mat3x3){1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f})
#define ID4x4 ((mat4x4){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f})



#define VEC_ADD2(u, v) ((__typeof__(u)){(u).x + (v).x, (u).y + (v).y})
#define VEC_ADD3(u, v) ((__typeof__(u)){(u).x + (v).x, (u).y + (v).y, (u).z + (v).z})
#define VEC_ADD4(u, v) ((__typeof__(u)){(u).x + (v).x, (u).y + (v).y, (u).z + (v).z, (u).v + (v).v})

#define VEC_SUB2(u, v) ((__typeof__(u)){(u).x - (v).x, (u).y - (v).y})
#define VEC_SUB3(u, v) ((__typeof__(u)){(u).x - (v).x, (u).y - (v).y, (u).z - (v).z})
#define VEC_SUB4(u, v) ((__typeof__(u)){(u).x - (v).x, (u).y - (v).y, (u).z - (v).z, (u).v - (v).v})

#define VEC_SCA2(u, a) ((__typeof__(u)){(u).x * a, (u).y * a})
#define VEC_SCA3(u, a) ((__typeof__(u)){(u).x * a, (u).y * a, (u).z * a})
#define VEC_SCA4(u, a) ((__typeof__(u)){(u).x * a, (u).y * a, (u).z * a, (u).v * a})

#define VEC_DOT2(u, v) ((float) ((u).x * (v).x  +  (u).y * (v).y))
#define VEC_DOT3(u, v) ((float) ((u).x * (v).x  +  (u).y * (v).y + (u).z * (v).z))
#define VEC_DOT4(u, v) ((float) ((u).x * (v).x  +  (u).y * (v).y + (u).z * (v).z + (u).w * (v).w))

#define VEC_NORMSQ2(u) ((u).x * (u).x + (u).y * (u).y)
#define VEC_NORMSQ3(u) ((u).x * (u).x + (u).y * (u).y  + (u).z * (u).z)
#define VEC_NORMSQ4(u) ((u).x * (u).x + (u).y * (u).y  + (u).z * (u).z  + (u).w * (u).w)

#define VEC_NORMALIZE2(u) (VEC_SCA2(u, 1.0f/ sqrtf(VEC_NORMSQ2((u)))))
#define VEC_NORMALIZE3(u) (VEC_SCA3(u, 1.0f/ sqrtf(VEC_NORMSQ3((u)))))
#define VEC_NORMALIZE4(u) (VEC_SCA4(u, 1.0f/ sqrtf(VEC_NORMSQ4((u)))))

#define VEC_CROSS3(u, v) ((__typeof__(u)){			\
				(u).y * (v).z  -  (u).z * (v).y, 	\
				(u).z * (v).x  -  (u).x * (v).z,	\
				(u).x * (v).y  -  (u).y * (v).x 	\
			})



static inline vec3f _VEC_CROSS3(vec3f u, vec3f v) {

	return (vec3f){
		u.y * v.z  -  u.z * v.y,
		u.z * v.x  -  u.x * v.z,
		u.x * v.y  -  u.y * v.x
	};
}



static inline mat2x2 mat2x2_ADD(mat2x2 A, mat2x2 B) {

	return (mat2x2){
		A.a0 + B.a0, A.a1 * B.a1,
		A.b0 * B.b0, A.b1 * B.b1,
	};
}

static inline mat3x3 mat3x3_ADD(mat3x3 A, mat3x3 B) {

	return (mat3x3){
		A.a0 + B.a0, A.a1 * B.a1, A.a2 + B.a2,
		A.b0 * B.b0, A.b1 * B.b1, A.b2 + B.b2,
		A.c0 + B.c0, A.c1 + B.c1, A.c2 + B.c2
	};
}

static inline mat4x4 mat4x4_ADD(mat4x4 A, mat4x4 B) {

	return (mat4x4){
		A.a0 + B.a0, A.a1 * B.a1, A.a2 + B.a2, A.a3 + B.a3,
		A.b0 * B.b0, A.b1 * B.b1, A.b2 + B.b2, A.b3 + B.b3,
		A.c0 + B.c0, A.c1 + B.c1, A.c2 + B.c2, A.c3 + B.c3,
		A.d0 + B.d0, A.d1 + B.d1, A.d2 + B.d2, A.d3 + B.d3
	};
}


static inline mat2x2 mat2x2_SCA(mat2x2 A, float c) {

	return (mat2x2){
		A.a0 * c, A.a1 * c,
		A.b0 * c, A.b1 * c,
	};
}


static inline mat3x3 mat3x3_SCA(mat3x3 A, float c) {

	return (mat3x3){
		A.a0 * c, A.a1 * c, A.a2 * c,
		A.b0 * c, A.b1 * c, A.b2 * c,
		A.c0 * c, A.c1 * c, A.c2 * c,
	};
}


static inline mat4x4 mat4x4_SCA(mat4x4 A, float c) {

	return (mat4x4){
		A.a0 * c, A.a1 * c, A.a2 * c, A.a3 * c,
		A.b0 * c, A.b1 * c, A.b2 * c, A.b3 * c,
		A.c0 * c, A.c1 * c, A.c2 * c, A.c3 * c,
		A.d0 * c, A.d1 * c, A.d2 * c, A.d3 * c
	};
}


static inline mat2x2 mat2x2_MUL(mat2x2 A, mat2x2 B) {

	return (mat2x2){
		A.a0*B.a0 + A.a1*B.b0, 	A.a0*B.a1 + A.a1*B.b1,
		A.b0*B.a0 + A.b1*B.b0, 	A.b0*B.a1 + A.b1*B.b1
	};
}

static inline vec2f mat2x2_V2D_MUL(mat2x2 A, vec2f v) {
	
	return (vec2f) {
		A.a0*v.x + A.a1*v.y,
		A.b0*v.x + A.b1*v.y
	};
}

static inline float mat2x2_V1D_MUL(mat2x2 A, float x) {
	return A.a0*x + A.a1;
}

static inline mat3x3 mat3x3_MUL(mat3x3 A, mat3x3 B) {
	
	return (mat3x3) {
		A.a0*B.a0 + A.a1*B.b0 + A.a2*B.c0,	A.a0*B.a1 + A.a1*B.b1 + A.a2*B.c1,	A.a0*B.a2 + A.a1*B.b2 + A.a2*B.c2,
		A.b0*B.a0 + A.b1*B.b0 + A.b2*B.c0,	A.b0*B.a1 + A.b1*B.b1 + A.b2*B.c1,	A.b0*B.a2 + A.b1*B.b2 + A.b2*B.c2,
		A.c0*B.a0 + A.c1*B.b0 + A.c2*B.c0,	A.c0*B.a1 + A.c1*B.b1 + A.c2*B.c1,	A.c0*B.a2 + A.c1*B.b2 + A.c2*B.c2
	};
}

static inline vec3f mat3x3_V3D_MUL(mat3x3 A, vec3f v) {
	
	return (vec3f) {
		A.a0*v.x + A.a1*v.y + A.a2*v.z,
		A.b0*v.x + A.b1*v.y + A.b2*v.z,
		A.c0*v.x + A.c1*v.y + A.c2*v.z,
	};
}

static inline vec2f mat3x3_V2D_MUL(mat3x3 A, vec2f v) {
	
	return (vec2f) {
		A.a0*v.x + A.a1*v.y + A.a2,
		A.b0*v.x + A.b1*v.y + A.b2
	};
}


static inline mat4x4 mat4x4_MUL(mat4x4 A, mat4x4 B) {

	return (mat4x4)	{
		A.a0*B.a0 + A.a1*B.b0 + A.a2*B.c0 + A.a3*B.d0,	A.a0*B.a1 + A.a1*B.b1 + A.a2*B.c1 + A.a3*B.d1,	A.a0*B.a2 + A.a1*B.b2 + A.a2*B.c2 + A.a3*B.d2,	A.a0*B.a3 + A.a1*B.b3 + A.a2*B.c3 + A.a3*B.d3,
		A.b0*B.a0 + A.b1*B.b0 + A.b2*B.c0 + A.b3*B.d0,	A.b0*B.a1 + A.b1*B.b1 + A.b2*B.c1 + A.b3*B.d1,	A.b0*B.a2 + A.b1*B.b2 + A.b2*B.c2 + A.b3*B.d2,	A.b0*B.a3 + A.b1*B.b3 + A.b2*B.c3 + A.b3*B.d3,
		A.c0*B.a0 + A.c1*B.b0 + A.c2*B.c0 + A.c3*B.d0,	A.c0*B.a1 + A.c1*B.b1 + A.c2*B.c1 + A.c3*B.d1,	A.c0*B.a2 + A.c1*B.b2 + A.c2*B.c2 + A.c3*B.d2,	A.c0*B.a3 + A.c1*B.b3 + A.c2*B.c3 + A.c3*B.d3,
		A.d0*B.a0 + A.d1*B.b0 + A.d2*B.c0 + A.d3*B.d0,	A.d0*B.a1 + A.d1*B.b1 + A.d2*B.c1 + A.d3*B.d1,	A.d0*B.a2 + A.d1*B.b2 + A.d2*B.c2 + A.d3*B.d2,	A.d0*B.a3 + A.d1*B.b3 + A.d2*B.c3 + A.d3*B.d3
	};
}

static inline vec4f mat4x4_V4D_MUL(mat4x4 A, vec4f u) {

	return (vec4f){
		A.a0*u.x + A.a1*u.y + A.a2*u.z + A.a3*u.w,
		A.b0*u.x + A.b1*u.y + A.b2*u.z + A.b3*u.w,
		A.c0*u.x + A.c1*u.y + A.c2*u.z + A.c3*u.w,
		A.d0*u.x + A.d1*u.y + A.d2*u.z + A.d3*u.w
	};
}

static inline vec3f mat4x4_V3D_MUL(mat4x4 A, vec3f u) {

    return (vec3f){
		A.a0*u.x + A.a1*u.y + A.a2*u.z + A.a3,
		A.b0*u.x + A.b1*u.y + A.b2*u.z + A.b3,
		A.c0*u.x + A.c1*u.y + A.c2*u.z + A.c3
	};
}

static inline vec3f mat4x4_V3D_WDIV_MUL(mat4x4 A, vec3f u) {

	float w = A.d0*u.x + A.d1*u.y + A.d2*u.z + A.d3;

	//DONT DIVIDE BY ZERO !!
    if (!w) return NUL3f;
    else return (vec3f){
		(A.a0*u.x + A.a1*u.y + A.a2*u.z + A.a3) / w,
		(A.b0*u.x + A.b1*u.y + A.b2*u.z + A.b3) / w,
		(A.c0*u.x + A.c1*u.y + A.c2*u.z + A.c3) / w
	};
}




static inline float mat2x2_DET(mat2x2 A) {
	return A.a0*A.b1 - A.a1*A.b0;
}

static inline float mat3x3_DET(mat3x3 A) {
	return 
		A.a0*(A.b1*A.c2	- A.b2*A.c1) -
	   	A.a1*(A.b0*A.c2	- A.b2*A.c1) +
	   	A.a2*(A.b0*A.c1	- A.b1*A.c1);
}

// static inline float mat4x4_DET(mat4x4 A) {
// 	return 1.0f;

// }

static inline mat2x2 mat2x2_INV(mat2x2 A) {

	float d = mat2x2_DET(A);

	return (mat2x2) {
		A.b1/d, -A.a1/d,
	   -A.b0/d,  A.a0/d

	};
}

static inline mat3x3 mat3x3_INV(mat3x3 A) {

	float d = mat3x3_DET(A);

	return (mat3x3) {
		(A.c2*A.b1 - A.c1*A.b2)/d, 	-(A.c2*A.a1 - A.c1*A.a2)/d,	  (A.b2*A.a1 - A.b1*A.a2)/d,
	   -(A.c2*A.b0 - A.c0*A.b2)/d,	 (A.c2*A.a0 - A.c0*A.a2)/d,	 -(A.b2*A.a0 - A.b0*A.a2)/d,
		(A.c1*A.b0 - A.c0*A.b1)/d,	-(A.c1*A.a0 - A.c0*A.a1)/d,   (A.b1*A.a0 - A.b0*A.a1)/d
	};
}


// static inline mat4x4 mat4x4_INV(mat4x4 A) {
// 	return ID4x4;
// }




//wikipedia affine transforms
#define AFF_ROT(u)  ((mat3x3){				\
					cosf(u), sinf(u), 0.0f, \
				   -sinf(u), cosf(u), 0.0f, \
					0.0f, 	 0.0f, 	  1.0f	\
					})

#define AFF_SCALE(x,y) ((mat3x3){		  \
						x, 	  0.0f,	0.0f, \
						0.0f, y, 	0.0f, \
						0.0f, 0.0f, 1.0f  \
					})

#define AFF_SHEAR(x,y) ((mat3x3){		  \
						1.0f, x,    0.0f, \
						y,    1.0f, 0.0f, \
						0.0f, 0.0f, 1.0f  \
					})

#define AFF_TRANS(x,y) ((mat3x3){		 \
						1.0f, 0.0f, x,	 \
						0.0f, 1.0f, y, 	 \
						0.0f, 0.0f, 1.0f \
					})



//https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix
#define PROJ_PERSPECTIVE(fov, a, zn, zf)({	\
		float f = 1.0f / tanf(0.5f*(fov));  \
  		float q = (zf) / ((zf) - (zn));		\
  											\
		(mat4x4){							\
		f,     0.0f,  0.0f,    0.0f, 		\
		0.0f,  f*(a), 0.0f,    0.0f, 		\
		0.0f,  0.0f,  q, 	   (zn)*q, 		\
		0.0f,  0.0f,  1.0,     1.0f};		})


//orthographic projection



//https://www.tutorialspoint.com/computer_graphics/3d_transformation.htm
#define TRANS_TRANS(x, y, z)((mat4x4){		\
		1.0f,   0.0f,   0.0f,   x,	  		\
		0.0f,   1.0f,   0.0f,   y, 			\
		0.0f,   0.0f,   1.0f,   z, 			\
		0.0f,   0.0f,   0.0f,   1.0f		})


#define TRANS_SCALE(x, y, z)((mat4x4){		\
		x,      0.0f,   0.0f,   0.0f,  		\
		0.0f,   y,      0.0f,   0.0f,		\
		0.0f,   0.0f,   z,      0.0f,		\
		0.0f,   0.0f,   0.0f,   1.0f		})


#define TRANS_ROTX(u)((mat4x4){		\
		1.0f,    0.0f,    0.0f,    0.0f,	\
		0.0f,    cosf(u), sinf(u), 0.0f,	\
		0.0f,   -sinf(u), cosf(u), 0.0f,	\
		0.0f,    0.0f,    0.0f,    1.0f		})

#define TRANS_ROTY(u)((mat4x4){		\
		cosf(u), 0.0f,   -sinf(u), 0.0f,	\
		0.0f,    1.0f,    0.0f,    0.0f,	\
		sinf(u), 0.0f, 	  cosf(u), 0.0f,	\
		0.0f,    0.0f,    0.0f,    1.0f		})

#define TRANS_ROTZ(u)((mat4x4){		\
		cosf(u), sinf(u), 0.0f,    0.0f,	\
	   -sinf(u), cosf(u), 0.0f,    0.0f,	\
		0.0f, 	 0.0f, 	  1.0f,	   0.0f,	\
		0.0f,    0.0f,    0.0f,    1.0f		})


#define TRANS_ROTZ(u)((mat4x4){		\
		cosf(u), sinf(u), 0.0f,    0.0f,	\
	   -sinf(u), cosf(u), 0.0f,    0.0f,	\
		0.0f, 	 0.0f, 	  1.0f,	   0.0f,	\
		0.0f,    0.0f,    0.0f,    1.0f		})


/*
#define TRANS_POINTAT(f, r, u, t)((mat4x4){	\
		(f).x, (r).x, (u).x, (t).x,			\
		(f).y, (r).y, (u).y, (t).y,			\
		(f).z, (r).z, (u).z, (t).z,			\
		0.0f,  0.0f,  0.0f,  1.0f	})

//inverse of POINTAT
static inline mat4x4 TRANS_LOOKAT(f, r, u, t) {
	return (mat4x4){							
		(f).x, (f).y, (f).z, -VEC_DOT3(f, t),
		(r).x, (r).y, (r).z, -VEC_DOT3(r, t),
		(u).x, (u).y, (u).z, -VEC_DOT3(u, t),
		0.0f,  0.0f,  0.0f,  1.0f		
	};
}
*/
	



#define MPRINT3x3(a) printf("| %+3.2f   %+3.2f   %+3.2f |\n| %+3.2f   %+3.2f   %+3.2f |\n| %+3.2f   %+3.2f   %+3.2f |\n",	\
							(a).a0, (a).a1, (a).a2, (a).b0, (a).b1, (a).b2, (a).c0, (a).c1, (a).c2);

#define MPRINT4x4(a) printf("| %+3.2f   %+3.2f   %+3.2f   %+3.2f |\n| %+3.2f   %+3.2f   %+3.2f   %+3.2f |\n| %+3.2f   %+3.2f   %+3.2f   %+3.2f |\n| %+3.2f   %+3.2f   %+3.2f   %+3.2f |\n",	\
							(a).a0, (a).a1, (a).a2, (a).a3, (a).b0, (a).b1, (a).b2, (a).b3, (a).c0, (a).c1, (a).c2, (a).c3, (a).d0, (a).d1, (a).d2, (a).d3);




#endif