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
static void _normal(EZ_Vec_t* res, EZ_Vec_t* p1, EZ_Vec_t* p2, EZ_Vec_t* p3) {
	
	EZ_Vec_t v1, v2, normal;

	EZ_vec_sub(&v1, p3, p1);
	EZ_vec_sub(&v2, p2, p1);

	EZ_vec_cross(&normal, &v1, &v2);
	EZ_vec_normal(res, &normal);
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



void EZ_draw3D_tri(EZ_3DTarget_t* tgt, EZ_Image_t* tex, EZ_Tri_t* tri, EZ_Mat4_t* trns) {

	#define X0 (screen_pos[0].x)
	#define Y0 (screen_pos[0].y)
	#define U0 (screen_pos[0].u)
	#define V0 (screen_pos[0].v)
	#define Z0 (screen_pos[0].z)

	#define X1 (screen_pos[1].x)
	#define Y1 (screen_pos[1].y)
	#define U1 (screen_pos[1].u)
	#define V1 (screen_pos[1].v)
	#define Z1 (screen_pos[1].z)

	#define X2 (screen_pos[2].x)
	#define Y2 (screen_pos[2].y)
	#define U2 (screen_pos[2].u)
	#define V2 (screen_pos[2].v)
	#define Z2 (screen_pos[2].z)

	int i;

	struct {
		int x, y;
		float u, v, z;
	} screen_pos[3];

	EZ_Vec_t transformed[3], projected[3];


	/* transform  -  apply transform matrix */
	for (i = 0; i < 3; i++) {
		EZ_mat4_vmul(&transformed[i], trns, &(tri->vert[i].pos) );

		/* don't render faces that are behind the camera */
		if (transformed[i].z < 0.0) 
			return;
	}


	/* project to 2D */
	for (i = 0; i < 3; i++) {

		/* apply projection matrix */
		EZ_mat4_vmul(&projected[i], tgt->proj, &transformed[i]);


		/* non linear part of the projection */
		/* don't divide by 0 */
		if (projected[i].w == 0.0) return;

		const float z = 1.0 / projected[i].w;

		EZ_vec_scale(&projected[i], &projected[i], z);


		/* convert to pixel space */
		/* from -1 to +1 with y from bottom to top */
		screen_pos[i].x = (projected[i].x + 1.0f) * 0.5f * tgt->img->w;
		screen_pos[i].y = (projected[i].y + 1.0f) * 0.5f * tgt->img->h;
		screen_pos[i].u = tri->vert[i].u;
		screen_pos[i].v = tri->vert[i].v;
		screen_pos[i].z = z;

		if (tgt->do_uv_correction) {
			screen_pos[i].u *= z;
			screen_pos[i].v *= z;
		}
	}



	/* culling */
	/* accurate according to the given perspective matrix */
	/* projected normal z componant  */
	/* allow to use other matrices */
	const float nz = (projected[1].x - projected[0].x) * (projected[2].y - projected[0].y) -
				     (projected[1].y - projected[0].y) * (projected[2].x - projected[0].x);

	if (nz > 0.0f) return;
	

	/* compute world space normal */
	_normal(&tri->normal, &transformed[0], &transformed[1], &transformed[2]);

	/* light and shading */
	EZ_Vec_t l_dir = {{0.0f, 0.0f, 1.0f}};
	float illum = CLAMP(EZ_vec_dot(&tri->normal, &l_dir), 0.0, 1.0);


	/* sort for drawing */
	if (Y0 > Y1) {
		SWAP(screen_pos[0],  screen_pos[1]);
	}
	if (Y0 > Y2) {
		SWAP(screen_pos[0],  screen_pos[2]);
	}
	if (Y1 > Y2) {
		SWAP(screen_pos[1],  screen_pos[2]);
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

			if (tgt->zbuff[x + y * tgt->img->w] > z) {
				continue;
			}
			else {
				tgt->zbuff[x + y * tgt->img->w] = z;
			}

			EZ_Px_t* px = tgt->img->px + (x + y * tgt->img->w);

			if (tgt->do_uv_correction) {
				tgt->shader(px, tex, tri->col, illum, u/z, v/z, z);
			}
			else {
				tgt->shader(px, tex, tri->col, illum, u, v, z);	
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




void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns) {
	int i;

	/* draw each triangle */
	for (i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_tri(tgt, mesh->texture, mesh->triangles + i, trns);
	}
}
