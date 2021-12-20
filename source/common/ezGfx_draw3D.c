#include "ezGfx_draw3D.h"
#include "ezGfx_draw2D.h"
#include "ezGfx_utils.h"

#include <stdlib.h>
#include <float.h>


EZ_3DTarget_t* EZ_draw3D_makeTarget(EZ_Image_t* img, EZ_Mat4_t* proj, EZ_Mat4_t* trns) {
	
	EZ_3DTarget_t* tgt = malloc( sizeof(EZ_3DTarget_t) );

	tgt->img = img;
	tgt->proj = proj;
	tgt->trns = trns;

	tgt->zbuff = malloc( img->w * img->h * sizeof(float) );

	return tgt;
}

void EZ_draw3D_freeTarget(EZ_3DTarget_t* tgt) {
	free(tgt->zbuff);
	free(tgt);
}


void EZ_draw3D_startScene(EZ_3DTarget_t* tgt) {
	int x, y;

	/* fill everything with default value */
	for (y = 0; y < tgt->img->h; y++)
	for (x = 0; x < tgt->img->w; x++) {

		tgt->zbuff[x + y * tgt->img->w] = 0.0;
	}

}




/* normal of a plane defined by 3 points by the right hand rule */
/* p1->p2 : index, p1->p3 : middle, normal : thumb */
static void _normal(EZ_Vec_t* res, EZ_Vec_t* p1, EZ_Vec_t* p2, EZ_Vec_t* p3) {
	
	EZ_Vec_t v1, v2, normal;

	EZ_vec_sub(&v1, p3, p1);
	EZ_vec_sub(&v2, p2, p1);

	EZ_vec_cross(&normal, &v1, &v2);
	EZ_vec_normal(res, &normal);
}

typedef struct {
	int x, y;
	float u, v, z;
} screen_pos;


#define PROJECT_SUCCESS 0
#define PROJECT_BEHIND  1
#define PROJECT_AWAY    2
#define PROJECT_DIVZERO 3


static int _projtri(EZ_3DTarget_t* tgt, screen_pos res[3], EZ_Tri_t* tri, EZ_Mat4_t* trns) {

	int i;

	EZ_Vec_t transformed[3], projected[3];


	/* transform  -  apply transform matrix */
	for (i = 0; i < 3; i++) {
		EZ_mat4_vmul(&transformed[i], trns, &(tri->vert[i].pos) );

		/* don't render faces that are behind the camera */
		if (transformed[i].z < 0.0) 
			return PROJECT_BEHIND;
	}


	/* project to 2D */
	for (i = 0; i < 3; i++) {

		/* apply projection matrix */
		EZ_mat4_vmul(&projected[i], tgt->proj, &transformed[i]);


		/* non linear part of the projection */
		/* don't divide by 0 */
		if (projected[i].w == 0.0) 
			return PROJECT_DIVZERO;


		const float z = 1.0 / projected[i].w;

		EZ_vec_scale(&projected[i], &projected[i], z);


		/* convert to pixel space */
		/* from -1 to +1 with y from bottom to top */
		res[i].x = (projected[i].x + 1.0f) * 0.5f * tgt->img->w;
		res[i].y = (projected[i].y + 1.0f) * 0.5f * tgt->img->h;
		res[i].u = tri->vert[i].u;
		res[i].v = tri->vert[i].v;
		res[i].z = z;
	}


	/* culling */
	/* accurate according to the given perspective matrix */
	/* projected normal z componant  */
	/* allow to use other matrices */
	const float nz = (projected[1].x - projected[0].x) * (projected[2].y - projected[0].y) -
				     (projected[1].y - projected[0].y) * (projected[2].x - projected[0].x);

	if (nz > 0.0f) return PROJECT_BEHIND;
	

	/* compute world space normal */
	_normal(&tri->normal, &transformed[0], &transformed[1], &transformed[2]);

	/* light and shading */
	EZ_Vec_t l_dir = {{0.0f, 0.0f, 1.0f}};
	tri->illum = CLAMP(EZ_vec_dot(&tri->normal, &l_dir), 0.0, 1.0);

	/* sort for drawing */
	if (res[0].y > res[1].y) {
		SWAP(res[0],  res[1]);
	}
	if (res[0].y > res[2].y) {
		SWAP(res[0],  res[2]);
	}
	if (res[1].y > res[2].y) {
		SWAP(res[1],  res[2]);
	}


	return PROJECT_SUCCESS;

}

void EZ_draw3D_wireTri(EZ_3DTarget_t* tgt, EZ_Tri_t* tri, EZ_Mat4_t* trns) {


	screen_pos vtx[3];

	if (PROJECT_SUCCESS != _projtri(tgt, vtx, tri, trns)) return;


	float dxdy1 = (float)(vtx[1].x - vtx[0].x)/(vtx[1].y - vtx[0].y);
	float dxdy2 = (float)(vtx[2].x - vtx[0].x)/(vtx[2].y - vtx[0].y);

	float x_start = vtx[0].x;
	float x_end   = vtx[0].x;

	/* scan lines */
	for (int y = vtx[0].y; y < vtx[2].y; y++) {

		/* switch to bottom half of the triangle */
		if (y == vtx[1].y) {
			
			dxdy1 = (float)(vtx[2].x - vtx[1].x)/(vtx[2].y - vtx[1].y);
			x_start = vtx[1].x;

		}


		tgt->img->px[(int)x_start + y * tgt->img->w] = tri->col;
		tgt->img->px[(int)x_end   + y * tgt->img->w] = tri->col;


		x_start += dxdy1;
		x_end   += dxdy2;

	}
	
}

void EZ_draw3D_flatTri(EZ_3DTarget_t* tgt, EZ_Tri_t* tri, EZ_Mat4_t* trns) {


	screen_pos vtx[3];

	if (PROJECT_SUCCESS != _projtri(tgt, vtx, tri, trns)) return;


	float dxdy1 = (float)(vtx[1].x - vtx[0].x)/(vtx[1].y - vtx[0].y);
	float dxdy2 = (float)(vtx[2].x - vtx[0].x)/(vtx[2].y - vtx[0].y);

	float x_start = vtx[0].x;
	float x_end   = vtx[0].x;

	/* scan lines */
	for (int y = vtx[0].y; y < vtx[2].y; y++) {

		/* switch to bottom half of the triangle */
		if (y == vtx[1].y) {
			dxdy1 = (float)(vtx[2].x - vtx[1].x)/(vtx[2].y - vtx[1].y);
			x_start = vtx[1].x;
		}

		int x_left, x_right;

		if (x_start < x_end) {
			x_left  = x_start;
			x_right = x_end;
		} 
		else {
			x_left  = x_end;
			x_right = x_start;
		}

		for (int x = x_left; x < x_right; x++) {

			EZ_Px_t px = tri->col;

			px.r *= tri->illum;
			px.g *= tri->illum;
			px.b *= tri->illum;

			tgt->img->px[x + y * tgt->img->w] = px;
		}

		x_start += dxdy1;
		x_end   += dxdy2;

	}

}

void EZ_draw3D_texTri(EZ_3DTarget_t* tgt, EZ_Image_t* tex, EZ_Tri_t* tri, EZ_Mat4_t* trns) { 


	screen_pos vtx[3];

	if (PROJECT_SUCCESS != _projtri(tgt, vtx, tri, trns)) return;


	float dxdy1 = (float)(vtx[1].x - vtx[0].x)/(vtx[1].y - vtx[0].y);
	float dxdy2 = (float)(vtx[2].x - vtx[0].x)/(vtx[2].y - vtx[0].y);

	float dudy1 = (float)(vtx[1].u - vtx[0].u)/(vtx[1].y - vtx[0].y);
	float dudy2 = (float)(vtx[2].u - vtx[0].u)/(vtx[2].y - vtx[0].y);

	float dvdy1 = (float)(vtx[1].v - vtx[0].v)/(vtx[1].y - vtx[0].y);
	float dvdy2 = (float)(vtx[2].v - vtx[0].v)/(vtx[2].y - vtx[0].y);


	float x_start = vtx[0].x;
	float x_end   = vtx[0].x;

	float u_start = vtx[0].u;
	float u_end   = vtx[0].u;
			
	float v_start = vtx[0].v;
	float v_end   = vtx[0].v;


	/* scan lines */
	for (int y = vtx[0].y; y < vtx[2].y; y++) {

		/* switch to bottom half of the triangle */
		if (y == vtx[1].y) {
			
			dxdy1 = (float)(vtx[2].x - vtx[1].x)/(vtx[2].y - vtx[1].y);
			dudy1 = (float)(vtx[2].u - vtx[1].u)/(vtx[2].y - vtx[1].y);
			dvdy1 = (float)(vtx[2].v - vtx[1].v)/(vtx[2].y - vtx[1].y);

			x_start = vtx[1].x;
			u_start = vtx[1].u;
			v_start = vtx[1].v;

		}


		float dudx = (float)(u_end - u_start)/(x_end - x_start);
		float dvdx = (float)(v_end - v_start)/(x_end - x_start);

		/* sort */
		int x_left, x_right;
		float u_left, v_left;

		if (x_start < x_end) {
			x_left  = x_start;
		    x_right = x_end;
		   	u_left  = u_start;
		    v_left  = v_start;
		} 
		else {
			x_left  = x_end;
		    x_right = x_start;
		    u_left  = u_end;
		    v_left  = v_end;
		}


		/* draw line */
		float u = u_left;
		float v = v_left;

		for (int x = x_left; x < x_right; x++) {

			EZ_Px_t px = *EZ_image_samplef(tex, u, v);

			px.r *= tri->illum;
			px.g *= tri->illum;
			px.b *= tri->illum;

			tgt->img->px[x + y * tgt->img->w] = px;

			u += dudx;
			v += dvdx;
		}

		x_start += dxdy1;
		x_end   += dxdy2;

		u_start += dudy1;
		u_end   += dudy2;

		v_start += dvdy1;
		v_end   += dvdy2;
	}

}

void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns, EZ_3DMode_t mode) {
	int i;

	/* draw each triangle */
	for (i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_texTri(tgt, mesh->texture, mesh->triangles + i, trns);
	}
}
