#include "ezGfx_draw3D.h"
#include "ezGfx_draw2D.h"
#include "ezGfx_utils.h"

#include <stdlib.h>


void EZ_draw3D_drawMesh(EZ_Image_t* target, EZ_Mesh_t* mesh, EZ_Mat4_t* projection, EZ_Mat4_t* transform) {

	/* draw each triangle */
	for (int i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_tri(target, &mesh->triangles[i], projection, transform);
	}

}

/* normal of a plane defined by 3 points by the right hand rule */
/* p1->p2 : index, p1->p3 : middle, normal : thumb */
static inline void _normal(EZ_Vec_t* res, EZ_Vec_t* p1, EZ_Vec_t* p2, EZ_Vec_t* p3) {
	
	EZ_Vec_t v1, v2, normal;

	EZ_vec_sub(&v1, p3, p1);
	EZ_vec_sub(&v2, p2, p1);

	EZ_vec_cross(&normal, &v1, &v2);
	EZ_vec_normal(res, &normal);
}


static inline void _mulwdiv(EZ_Vec_t* C, EZ_Mat4_t* A, EZ_Vec_t* B) {
	double w = A->v[3][0] * B->v[0] 
	         + A->v[3][1] * B->v[1]
	         + A->v[3][2] * B->v[2]
	         + A->v[3][3];

	if (!w) {
		return; /* don't divide by 0 */
	}

	EZ_Vec_t proj;

	EZ_mat4_vdown(&proj, A, B);
	EZ_vec_scale(C, &proj, 1.0 / w);
}



void EZ_draw3D_tri(EZ_Image_t* target, EZ_Tri_t* tri,  EZ_Mat4_t* projection, EZ_Mat4_t* transform) {

	EZ_Vec_t proj[3]; /* holds the coordinates in screen space of the three vertices */

	/* transform  -  apply transform matrix */
	for (int j = 0; j < 3; j++)
		EZ_mat4_vdown(&proj[j], transform, &(tri->points[j].pos) );


	/* compute world space normal */
	EZ_Vec_t normal;
	_normal(&normal, &proj[0], &proj[1], &proj[2]);


	/* don't render faces that are behind the camera */
	if (proj[0].z < 0.0f || 
		proj[1].z < 0.0f || 
		proj[2].z < 0.0f) return;

	/*  if (proj[0].z < 0.0f &&  */
	/*  	proj[1].z < 0.0f && */
	/*  	proj[2].z < 0.0f) return; */


	/* project to 2D  - apply projection matrix */
	for (int j = 0; j < 3; j++)
		_mulwdiv(&proj[j], projection, &proj[j]);


	/* culling */
	/* accurate according to the given perspective matrix */
	/* projected normal z componant  */
	/* allow to use other matrices */
	const float nz = (proj[1].x - proj[0].x) * (proj[2].y - proj[0].y) -
				     (proj[1].y - proj[0].y) * (proj[2].x - proj[0].x);

	if (nz > 0.0f) return; /* right hand rule ? minus sign ? */
	

	/* convert to pixel space */
	/* from -1 to +1 with y from bottom to top */
	for (int j = 0; j < 3; j++) {
		proj[j].x += 1.0f; 
		proj[j].x *= 0.5f*target->w;

		proj[j].y += -1.0f; 
		proj[j].y *= -0.5f*target->h;

	}


	/* light and shading */
	EZ_Vec_t l_dir = {{0.0f, 0.0f, 1.0f}};
	float b  = CLAMP(EZ_vec_dot(&normal, &l_dir), 0.25f, 1.0f);

	EZ_Px_t color = tri->col;
	color.r *= b;
	color.g *= b;
	color.b *= b;



	/* draw the triangle */

	EZ_draw2D_fillTri(target, color, proj[0].x, proj[0].y, proj[1].x, proj[1].y, proj[2].x, proj[2].y);
	/* EZ_draw2D_tri(target, tri->col, proj[0].x, proj[0].y, proj[1].x, proj[1].y, proj[2].x, proj[2].y); */

}
