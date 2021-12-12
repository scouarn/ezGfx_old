#ifndef _EZGFX_VEC_H_
#define _EZGFX_VEC_H_

typedef union {
	struct {float x, y, z, w;};
	float v[4];
} EZ_Vec_t;

void  EZ_vec_copy  (EZ_Vec_t* res, EZ_Vec_t* u);
void  EZ_vec_add   (EZ_Vec_t* res, EZ_Vec_t* u, EZ_Vec_t* v);
void  EZ_vec_sub   (EZ_Vec_t* res, EZ_Vec_t* u, EZ_Vec_t* v);
void  EZ_vec_cross (EZ_Vec_t* res, EZ_Vec_t* u, EZ_Vec_t* v);
void  EZ_vec_scale (EZ_Vec_t* res, EZ_Vec_t* u, float x);
float EZ_vec_dot   (EZ_Vec_t* u, EZ_Vec_t* v);
float EZ_vec_normsq(EZ_Vec_t* u);
float EZ_vec_norm  (EZ_Vec_t* u);
void  EZ_vec_normal(EZ_Vec_t* res, EZ_Vec_t* u);


#endif /* ezGfx_vec_h */