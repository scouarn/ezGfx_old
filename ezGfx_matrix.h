#include "ezGfx_core.h"


/*
/
/	EZGFX ALGEBRA EXTENSION
/
/
/ SCOUARN, 2021
/
/	For 2D and 3D transforms.
/
/
/
*/


#ifndef EZ_DRAW2D
#define EZ_DRAW2D



typedef struct {
  float x;
  float y;
  float z;
  float w;
} EZ_vec;


typedef float EZ_mat3x3[3][3];
typedef float EZ_mat4x4[4][4];


EZ_vec EZ_vec_add  (EZ_vec, EZ_vec);
EZ_vec EZ_vec_scale(EZ_vec, float );
EZ_vec EZ_vec_cross(EZ_vec, EZ_vec);
float  EZ_vec_dot2d(EZ_vec, EZ_vec);
float  EZ_vec_dot3d(EZ_vec, EZ_vec);

EZ_mat3x3 EZ_mat3x3_matMul(EZ_mat3x3, EZ_mat3x3);
EZ_mat3x3 EZ_mat4x4_matMul(EZ_mat4x4, EZ_mat4x4);
EZ_vec    EZ_mat3x3_vecMul(EZ_mat3x3, EZ_vec);
EZ_vec    EZ_mat4x4_vecMul(EZ_mat4x4, EZ_vec);


#endif
