#include "ezGfx_vec.h"
#include <math.h>

#define DIM 4

void EZ_vec_copy(EZ_Vec_t* res, EZ_Vec_t* u) {
	int i;
	for (i = 0; i < DIM; i++)
		res->v[i] = u->v[i];
}

void EZ_vec_add(EZ_Vec_t* res, EZ_Vec_t* u, EZ_Vec_t* v) {
	int i;
	for (i = 0; i < DIM; i++)
		res->v[i] = u->v[i] + v->v[i];
}


void EZ_vec_sub(EZ_Vec_t* res, EZ_Vec_t* u, EZ_Vec_t* v) {
	int i;
	for (i = 0; i < DIM; i++)
		res->v[i] = u->v[i] - v->v[i];
}

void EZ_vec_cross(EZ_Vec_t* res, EZ_Vec_t* u, EZ_Vec_t* v) {
	
	res->x = u->y * v->z  -  u->z * v->y;
	res->y = u->z * v->x  -  u->x * v->z;
	res->z = u->x * v->y  -  u->y * v->x;
	res->w = 0.0;
}

void EZ_vec_scale(EZ_Vec_t* res, EZ_Vec_t* u, float x) {
	int i;
	for (i = 0; i < DIM; i++)
		res->v[i] = u->v[i] * x;
}


float EZ_vec_dot(EZ_Vec_t* u, EZ_Vec_t* v) {
	int i;
	float res = 0.0;

	for (i = 0; i < DIM; i++)
		res += u->v[i] * v->v[i];

	return res;
}

float EZ_vec_normsq(EZ_Vec_t* u) {
	return EZ_vec_dot(u, u);
}

float EZ_vec_norm(EZ_Vec_t* u) {
	return sqrtf( EZ_vec_normsq(u) );
}

void EZ_vec_normal(EZ_Vec_t* res, EZ_Vec_t* u) {
	float norm = EZ_vec_norm(u);
	if (norm != 0.0) EZ_vec_scale(res, u, 1.0/norm);
}
