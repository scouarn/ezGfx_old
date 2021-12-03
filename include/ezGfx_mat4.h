#ifndef _EZGFX_MAT4_H_
#define _EZGFX_MAT4_H_

#include "ezGfx_vec.h"

typedef struct {
	unsigned int rows, cols;
	double v[4][4];
} EZ_Mat4_t;

void EZ_mat4_mul  (EZ_Mat4_t* res, EZ_Mat4_t* A, EZ_Mat4_t* B);
void EZ_mat4_vmul (EZ_Vec_t* res,  EZ_Mat4_t* A, EZ_Vec_t* v);
void EZ_mat4_vdown(EZ_Vec_t* res,  EZ_Mat4_t* A, EZ_Vec_t* v);

void EZ_mat4_copy (EZ_Mat4_t* res, EZ_Mat4_t* A);
void EZ_mat4_add  (EZ_Mat4_t* res, EZ_Mat4_t* A, EZ_Mat4_t* B);
void EZ_mat4_sub  (EZ_Mat4_t* res, EZ_Mat4_t* A, EZ_Mat4_t* B);
void EZ_mat4_scale(EZ_Mat4_t* res, EZ_Mat4_t* A, double B);

double EZ_mat4_det(EZ_Mat4_t* A);
void EZ_mat4_inv(EZ_Mat4_t* res, EZ_Mat4_t* A);

void EZ_mat4_setId(EZ_Mat4_t* res);
void EZ_mat4_setProj(EZ_Mat4_t* res, float fov, float aspect_ratio, float z_near, float z_far);

void EZ_mat4_setTranslate(EZ_Mat4_t* res, float x, float y, float z);
void EZ_mat4_setScale(EZ_Mat4_t* res, float x, float y, float z);
void EZ_mat4_setRotX(EZ_Mat4_t* res, float a);
void EZ_mat4_setRotY(EZ_Mat4_t* res, float a);
void EZ_mat4_setRotZ(EZ_Mat4_t* res, float a);

void EZ_mat4_applyTranslate(EZ_Mat4_t* res, float x, float y, float z);
void EZ_mat4_applyScale(EZ_Mat4_t* res, float x, float y, float z);
void EZ_mat4_applyRotX(EZ_Mat4_t* res, float a);
void EZ_mat4_applyRotY(EZ_Mat4_t* res, float a);
void EZ_mat4_applyRotZ(EZ_Mat4_t* res, float a);

#endif /* ezGfx_mat4_h */