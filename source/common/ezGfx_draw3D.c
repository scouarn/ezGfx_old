#include "ezGfx_draw3D.h"
#include "ezGfx_draw2D.h"
#include "ezGfx_utils.h"

#include <stdlib.h>


void EZ_draw3D_drawMesh(EZ_Image_t* target, EZ_Mesh_t* mesh, const mat4x4* projection, const mat4x4* transform) {

	/* draw each triangle */
	for (int i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_tri(target, &mesh->triangles[i], projection, transform);
	}

}

/* normal of a plane defined by 3 points by the right hand rule */
/* p1->p2 : index, p1->p3 : middle, normal : thumb */
static inline vec3f _normal(vec3f p1, vec3f p2, vec3f p3) {

	vec3f v1 = VEC_SUB3(p3, p1);
	vec3f v2 = VEC_SUB3(p2, p1);

	vec3f normal = _VEC_CROSS3(v1, v2);

	return VEC_NORMALIZE3(normal);
}




void EZ_draw3D_tri(EZ_Image_t* target, EZ_Tri_t* tri,  const mat4x4* projection, const mat4x4* transform) {

	vec3f proj[3]; /* holds the coordinates in screen space of the three vertices */

	/* transform  -  apply transform matrix */
	for (int j = 0; j < 3; j++)
		proj[j] = mat4x4_V3D_MUL(*transform, tri->points[j].pos);


	/* compute world space normal */
	const vec3f normal = _normal(proj[0], proj[1], proj[2]);


	/* don't render faces that are behind the camera */
	if (proj[0].z < 0.0f || 
		proj[1].z < 0.0f || 
		proj[2].z < 0.0f) return;

	/*  if (proj[0].z < 0.0f &&  */
	/*  	proj[1].z < 0.0f && */
	/*  	proj[2].z < 0.0f) return; */


	/* project to 2D  - apply projection matrix */
	for (int j = 0; j < 3; j++)
		proj[j] = mat4x4_V3D_WDIV_MUL(*projection, proj[j]);


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
	vec3f l_dir = {0.0f, 0.0f, 1.0f};
	float b  = CLAMP(VEC_DOT3(normal, l_dir), 0.25f, 1.0f);

	EZ_Px_t color = tri->col;
	color.r *= b;
	color.g *= b;
	color.b *= b;



	/* draw the triangle */

	EZ_draw2D_fillTri(target, color, proj[0].x, proj[0].y, proj[1].x, proj[1].y, proj[2].x, proj[2].y);
	/* EZ_draw2D_tri(target, tri->col, proj[0].x, proj[0].y, proj[1].x, proj[1].y, proj[2].x, proj[2].y); */

}
