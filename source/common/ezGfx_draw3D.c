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
	
	tgt->shader = EZ_draw3D_textureShader;
	tgt->do_uv_correction = true;


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

void EZ_draw3D_endScene(EZ_3DTarget_t* tgt) {

}


/* normal of a plane defined by 3 points by the right hand rule */
/* p1->p2 : index, p1->p3 : middle, normal : thumb */
static void _normal(EZ_Tri_t* tri) {
	
	EZ_Vec_t v1, v2, normal;

	EZ_Vec_t* p1 = &(tri->vert[0].pos);
	EZ_Vec_t* p2 = &(tri->vert[1].pos);
	EZ_Vec_t* p3 = &(tri->vert[2].pos);

	EZ_vec_sub(&v1, p3, p1);
	EZ_vec_sub(&v2, p2, p1);

	EZ_vec_cross(&normal, &v1, &v2);
	EZ_vec_normal(&tri->normal, &normal);
}



void EZ_draw3D_textureShader(EZ_Px_t* px, EZ_Image_t* tex, EZ_Px_t col, float illum, float u, float v, float z) {

	if (tex == NULL) *px = EZ_MAGENTA;

	EZ_Px_t* sample = EZ_image_samplef(tex, u, v);

	/* apply shading */
	px->r = sample->r * illum;
	px->g = sample->g * illum;
	px->b = sample->b * illum;

}



void EZ_draw3D_flatShader(EZ_Px_t* px, EZ_Image_t* tex, EZ_Px_t col, float illum, float u, float v, float z) {

	/* apply shading */
	px->r = col.r * illum;
	px->g = col.g * illum;
	px->b = col.b * illum;

}



#define X0 (tri->vert[0].sx)
#define Y0 (tri->vert[0].sy)
#define U0 (tri->vert[0].uv.x)
#define V0 (tri->vert[0].uv.y)
#define Z0 (tri->vert[0].uv.z)

#define X1 (tri->vert[1].sx)
#define Y1 (tri->vert[1].sy)
#define U1 (tri->vert[1].uv.x)
#define V1 (tri->vert[1].uv.y)
#define Z1 (tri->vert[1].uv.z)

#define X2 (tri->vert[2].sx)
#define Y2 (tri->vert[2].sy)
#define U2 (tri->vert[2].uv.x)
#define V2 (tri->vert[2].uv.y)
#define Z2 (tri->vert[2].uv.z)

#define WIDTH  (tgt->img->w)
#define HEIGHT (tgt->img->h)
#define PX (tgt->img->px)



static void _proj(EZ_3DTarget_t* tgt, EZ_Tri_t* tri, EZ_Mat4_t* trns) {

	int i;
	EZ_Vec_t projected[3];

	/* project to 2D */
	for (i = 0; i < 3; i++) {

		/* apply projection matrix */
		EZ_mat4_vmul(&projected[i], tgt->proj, &(tri->vert[i].pos) );

		/* non linear part of the projection */
		const float z = 1.0 / projected[i].w;

		EZ_vec_scale(&projected[i], &projected[i], z);

		/* convert to pixel space */
		/* from -1 to +1 with y from bottom to top */
		tri->vert[i].sx = (projected[i].x + 1.0f) * 0.5f * tgt->img->w;
		tri->vert[i].sy = (projected[i].y + 1.0f) * 0.5f * tgt->img->h;
		tri->vert[i].uv.z  = z;

		if (tgt->do_uv_correction) {
			tri->vert[i].uv.x *= z;
			tri->vert[i].uv.y *= z;
		}
	}


}

static void _raster(EZ_3DTarget_t* tgt, EZ_Image_t* tex, EZ_Tri_t* tri) {

	/* sort for drawing */
	if (Y0 > Y1) {
		SWAP(tri->vert[0],  tri->vert[1]);
	}
	if (Y0 > Y2) {
		SWAP(tri->vert[0],  tri->vert[2]);
	}
	if (Y1 > Y2) {
		SWAP(tri->vert[1],  tri->vert[2]);
	}


	/* compute slopes */
	float dxdy1 = (float)(X1 - X0)/(Y1 - Y0);
	float dxdy2 = (float)(X2 - X0)/(Y2 - Y0);

	float dudy1 = (float)(U1 - U0)/(Y1 - Y0);
	float dudy2 = (float)(U2 - U0)/(Y2 - Y0);

	float dvdy1 = (float)(V1 - V0)/(Y1 - Y0);
	float dvdy2 = (float)(V2 - V0)/(Y2 - Y0);

	float dzdy1 = (float)(Z1 - Z0)/(Y1 - Y0);
	float dzdy2 = (float)(Z2 - Z0)/(Y2 - Y0);

	float x_start = X0;
	float x_end   = X0;

	float u_start = U0;
	float u_end   = U0;
			
	float v_start = V0;
	float v_end   = V0;

	float z_start = Z0;
	float z_end   = Z0;


	/* scan lines */
	for (int y = Y0; y < Y2; y++) {

		/* switch to bottom half of the triangle */
		if (y == Y1) {
			
			dxdy1 = (float)(X2 - X1)/(Y2 - Y1);
			dudy1 = (float)(U2 - U1)/(Y2 - Y1);
			dvdy1 = (float)(V2 - V1)/(Y2 - Y1);
			dzdy1 = (float)(Z2 - Z1)/(Y2 - Y1);

			x_start = X1;  u_start = U1;  
			v_start = V1;  z_start = Z1;

		}

		/* interpolate uv along x */
		float dudx = (float)(u_end - u_start)/(x_end - x_start);
		float dvdx = (float)(v_end - v_start)/(x_end - x_start);
		float dzdx = (float)(z_end - z_start)/(x_end - x_start);


		/* sort from left to right */
		int x_left, x_right;
		float u, v, z;

		if (x_start < x_end) {
			x_left  = x_start;  
			x_right = x_end;

		   	u = u_start;  
		   	v = v_start;  
		   	z = z_start;
		} 
		else {
			x_left  = x_end;  
			x_right = x_start;

		    u = u_end;  
		    v = v_end;  
		    z = z_end;
		}


		/* draw line */
		for (int x = x_left; x < x_right; x++) {

			/* depth buffering */

			if (tgt->zbuff[x + y * WIDTH] > z) {
				continue;
			}
			else {
				tgt->zbuff[x + y * WIDTH] = z;
			}

			EZ_Px_t* px = PX + (x + y * WIDTH);

			if (tgt->do_uv_correction) {
				tgt->shader(px, tex, tri->col, tri->illum, u/z, v/z, z);
			}
			else {
				tgt->shader(px, tex, tri->col, tri->illum, u, v, z);	
			}

			u += dudx;  v += dvdx;  z += dzdx;
		}

		/* update floating coordinates */
		x_start += dxdy1;  x_end += dxdy2;
		u_start += dudy1;  u_end += dudy2;
		v_start += dvdy1;  v_end += dvdy2;
		z_start += dzdy1;  z_end += dzdy2;

	}


}


void EZ_draw3D_tri(EZ_3DTarget_t* tgt, EZ_Image_t* tex, EZ_Tri_t* tri_ori, EZ_Mat4_t* trns) {

	int i;
	EZ_Tri_t tri = *tri_ori; /* copy */

	/* transform */
	for (i = 0; i < 3; i++) {
		EZ_mat4_vmul( &(tri.vert[i].pos), trns, &(tri_ori->vert[i].pos) );

		/* don't render faces that are behind the camera */
		if (tri.vert[i].pos.z < 0.0) return;
	}

	/* DO Z CLIPPING */
		/* */
		/* */
		/* */


	/* compute world space normal */
	_normal(&tri);

	/* project */
	_proj(tgt, &tri, trns);


	/* culling -  projected normal z componant  */
	if ( (tri.vert[1].pos.x - tri.vert[0].pos.x) * (tri.vert[2].pos.y - tri.vert[0].pos.y)
	   - (tri.vert[1].pos.y - tri.vert[0].pos.y) * (tri.vert[2].pos.x - tri.vert[0].pos.x)
	   > 0.0f) 
		return;


	/* light and shading */
	EZ_Vec_t l_dir = {{0.0f, 0.0f, 1.0f}};
	tri.illum = CLAMP(EZ_vec_dot(&tri.normal, &l_dir), 0.0, 1.0);



	/* DO CLIPPING */
		/* */
		/* */
		/* */



	/* draw */
	_raster(tgt, tex, &tri);
}




void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns) {
	int i;

	/* draw each triangle */
	for (i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_tri(tgt, mesh->texture, mesh->triangles + i, trns);
	}
}
