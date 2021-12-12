#include "ezGfx_mat4.h"
#include <math.h>
#include <stdlib.h>

#define DIM 4

void EZ_mat4_mul(EZ_Mat4_t* res, EZ_Mat4_t* A, EZ_Mat4_t* B) {
	int i, j, k;
	EZ_Mat4_t* tmp = (res == A || res == B) ? malloc( sizeof(EZ_Mat4_t) ) 
	               : res;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		tmp->v[i][j] = 0.0;

		for (k = 0; k < DIM; k++)
			tmp->v[i][j] += A->v[i][k] * B->v[k][j];
	}


	if (res == A || res == B) {
		EZ_mat4_copy(res, tmp);
		free(tmp);
	}
}

void EZ_mat4_vmul(EZ_Vec_t*  res, EZ_Mat4_t* A, EZ_Vec_t* v) {
	int i, k;
	EZ_Vec_t* tmp = (res == v) ? malloc( sizeof(EZ_Vec_t) ) : res;
	
	for (i = 0; i < DIM; i++) {

		tmp->v[i] = 0.0;

		for (k = 0; k < DIM; k++)
			tmp->v[i] += A->v[i][k] * v->v[k];

	}

	if (res == v) {
		EZ_vec_copy(res, tmp);
		free(tmp);
	}
}


void EZ_mat4_vdown(EZ_Vec_t* res, EZ_Mat4_t* A, EZ_Vec_t* v) {
	int i, k;
	EZ_Vec_t* tmp = (res == v) ? malloc( sizeof(EZ_Vec_t) ) : res;
	
	for (i = 0; i < DIM-1; i++) {

		tmp->v[i] = A->v[i][DIM-1];

		for (k = 0; k < DIM-1; k++)
			tmp->v[i] += A->v[i][k] * v->v[k];

	}

	if (res == v) {
		EZ_vec_copy(res, tmp);
		free(tmp);
	}

}

void EZ_mat4_copy(EZ_Mat4_t* res, EZ_Mat4_t* A) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j];
	}
}

void EZ_mat4_add(EZ_Mat4_t* res, EZ_Mat4_t* A, EZ_Mat4_t* B) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j] + B->v[i][j];
	}
}

void EZ_mat4_sub(EZ_Mat4_t* res, EZ_Mat4_t* A, EZ_Mat4_t* B) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j] - B->v[i][j];
	}
}

void EZ_mat4_scale(EZ_Mat4_t* res, EZ_Mat4_t* A, float B) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j] * B;
	}
}

float EZ_mat4_det(EZ_Mat4_t* A) {return 0;}
void EZ_mat4_inv(EZ_Mat4_t* res, EZ_Mat4_t* A) {}


void EZ_mat4_setId(EZ_Mat4_t* res) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = (i == j) ? 1.0 : 0.0;
	}
}

void EZ_mat4_setProj(EZ_Mat4_t* res, float fov, float a, float zn, float zf) {
	float f = 1.0 / tanf(0.5*fov);
	float q = zf / (zf - zn);

	res->v[0][0] = f;   res->v[0][1] = 0.0; res->v[0][2] = 0.0; res->v[0][3] = 0.0; 
	res->v[1][0] = 0.0; res->v[1][1] = f*a; res->v[1][2] = 0.0; res->v[1][3] = 0.0; 
	res->v[2][0] = 0.0; res->v[2][1] = 0.0; res->v[2][2] = q;   res->v[2][3] = zn*q; 
	res->v[3][0] = 0.0; res->v[3][1] = 0.0; res->v[3][2] = 1.0; res->v[3][3] = 1.0; 
}


void EZ_mat4_setTranslate(EZ_Mat4_t* res, float x, float y, float z) {

	EZ_mat4_setId(res);

	res->v[0][3] = x; 
	res->v[1][3] = y; 
	res->v[2][3] = z; 
}


void EZ_mat4_setScale(EZ_Mat4_t* res, float x, float y, float z) {

	EZ_mat4_setId(res);

	res->v[0][0] = x;
	res->v[1][1] = y;
	res->v[2][2] = z;   
}

void EZ_mat4_setRotX(EZ_Mat4_t* res, float a) {

	EZ_mat4_setId(res);

	res->v[1][1] =  cos(a); res->v[1][2] = sin(a);
	res->v[2][1] = -sin(a); res->v[2][2] = cos(a);
}

void EZ_mat4_setRotY(EZ_Mat4_t* res, float a) {

	EZ_mat4_setId(res);
	
	res->v[0][0] = cos(a); res->v[0][2] = -sin(a);
	res->v[2][0] = sin(a); res->v[2][2] =  cos(a);
}

void EZ_mat4_setRotZ(EZ_Mat4_t* res, float a) {

	EZ_mat4_setId(res);
	
	res->v[0][0] =  cos(a); res->v[0][1] = sin(a);
	res->v[1][0] = -sin(a); res->v[1][1] = cos(a);
}



void EZ_mat4_applyTranslate(EZ_Mat4_t* res, float x, float y, float z) {

	EZ_Mat4_t mat;

	EZ_mat4_setTranslate(&mat, x, y, z);
	EZ_mat4_mul(res, res, &mat);
}

void EZ_mat4_applyScale(EZ_Mat4_t* res, float x, float y, float z) {
	
	EZ_Mat4_t mat;
	
	EZ_mat4_setScale(&mat, x, y, z);
	EZ_mat4_mul(res, res, &mat);
}

void EZ_mat4_applyRotX(EZ_Mat4_t* res, float a) {

	EZ_Mat4_t mat;
	
	EZ_mat4_setRotX(&mat, a);
	EZ_mat4_mul(res, res, &mat);
}

void EZ_mat4_applyRotY(EZ_Mat4_t* res, float a) {

	EZ_Mat4_t mat;
	
	EZ_mat4_setRotY(&mat, a);
	EZ_mat4_mul(res, res, &mat);
}

void EZ_mat4_applyRotZ(EZ_Mat4_t* res, float a) {

	EZ_Mat4_t mat;
	
	EZ_mat4_setRotZ(&mat, a);
	EZ_mat4_mul(res, res, &mat);
}