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
	
	res->v[0] = u->v[1] * v->v[2]  -  u->v[2] * v->v[1];
	res->v[1] = u->v[2] * v->v[0]  -  u->v[0] * v->v[2];
	res->v[2] = u->v[0] * v->v[1]  -  u->v[1] * v->v[0];
}

void EZ_vec_scale(EZ_Vec_t* res, EZ_Vec_t* u, double x) {
	int i;

	for (i = 0; i < DIM; i++)
		res->v[i] = u->v[i] * x;
}


double EZ_vec_dot(EZ_Vec_t* u, EZ_Vec_t* v) {
	int i;
	double res = 0.0;

	for (i = 0; i < DIM; i++)
		res += u->v[i] * v->v[i];

	return res;
}

double EZ_vec_normsq(EZ_Vec_t* u) {
	return EZ_vec_dot(u, u);
}

double EZ_vec_norm(EZ_Vec_t* u) {
	return sqrt( EZ_vec_normsq(u) );
}

void EZ_vec_normal(EZ_Vec_t* res, EZ_Vec_t* u) {
	EZ_vec_scale(res, u, EZ_vec_norm(u));
}
