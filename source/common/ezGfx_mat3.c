#include "ezGfx_mat3.h"
#include <math.h>
#include <stdlib.h>

#define DIM 3

void EZ_mat3_mul(EZ_Mat3_t* res, EZ_Mat3_t* A, EZ_Mat3_t* B) {
	int i, j, k;
	EZ_Mat3_t* tmp = (res == A || res == B) ? malloc( sizeof(EZ_Mat3_t) ) 
	               : res;


	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		tmp->v[i][j] = 0.0;

		for (k = 0; k < DIM; k++)
			tmp->v[i][j] += A->v[i][k] * B->v[k][j];
	}

	if (res == A || res == B) {
		EZ_mat3_copy(res, tmp);
		free(tmp);
	}

}


void EZ_mat3_vmul (EZ_Vec_t* res, EZ_Mat3_t* A, EZ_Vec_t* v) {
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


void EZ_mat3_vdown(EZ_Vec_t* res, EZ_Mat3_t* A, EZ_Vec_t* v) {
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

void EZ_mat3_copy(EZ_Mat3_t* res, EZ_Mat3_t* A) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j];
	}
}

void EZ_mat3_add(EZ_Mat3_t* res, EZ_Mat3_t* A, EZ_Mat3_t* B) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j] + B->v[i][j];
	}
}

void EZ_mat3_sub(EZ_Mat3_t* res, EZ_Mat3_t* A, EZ_Mat3_t* B) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j] - B->v[i][j];
	}
}

void EZ_mat3_scale(EZ_Mat3_t* res, EZ_Mat3_t* A, float B) {
	int i, j;

	for (i = 0; i < DIM; i++) 
	for (j = 0; j < DIM; j++) {

		res->v[i][j] = A->v[i][j] * B;
	}
}

float EZ_mat3_det(EZ_Mat3_t* A) {return 0;}
void EZ_mat3_inv(EZ_Mat3_t* res, EZ_Mat3_t* A) {}