#ifndef _EZGFX_MAT3_H_
#define _EZGFX_MAT3_H_

#include "ezGfx_vec.h"

typedef struct {
	unsigned int rows, cols;
	double v[3][3];
} EZ_Mat3_t;

void EZ_mat3_mul  (EZ_Mat3_t* res, EZ_Mat3_t* A, EZ_Mat3_t* B);
void EZ_mat3_vmul (EZ_Vec_t* res,  EZ_Mat3_t* A, EZ_Vec_t* v);
void EZ_mat3_vdown(EZ_Vec_t* res,  EZ_Mat3_t* A, EZ_Vec_t* v);

void EZ_mat3_copy (EZ_Mat3_t* res, EZ_Mat3_t* A);
void EZ_mat3_add  (EZ_Mat3_t* res, EZ_Mat3_t* A, EZ_Mat3_t* B);
void EZ_mat3_sub  (EZ_Mat3_t* res, EZ_Mat3_t* A, EZ_Mat3_t* B);
void EZ_mat3_scale(EZ_Mat3_t* res, EZ_Mat3_t* A, double B);

double EZ_mat3_det(EZ_Mat3_t* A);
void EZ_mat3_inv(EZ_Mat3_t* res, EZ_Mat3_t* A);


#endif /* ezGfx_mat3_h */