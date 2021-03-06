#include "ezGfx_draw3D.h"
#include "ezGfx_utils.h"
#include "ezErr.h"

#include <stdlib.h>


EZ_3DTarget_t* EZ_draw3D_makeTarget(EZ_Image_t* img, EZ_Mat4_t* proj, EZ_Mat4_t* trns) {
	
	EZ_3DTarget_t* tgt = malloc( sizeof(EZ_3DTarget_t) );

	tgt->img = img;
	tgt->proj = proj;
	tgt->trns = trns;
	tgt->faces = NULL;
	
	tgt->do_uv_correction = true;


	tgt->zbuff = malloc( img->w * img->h * sizeof(float) );

	return tgt;
}


static void _free_tri_list(EZ_3DTarget_t* tgt) {

	if (tgt == NULL) return;

	while (tgt->faces != NULL) {

		EZ_Tri_list* next = tgt->faces->next;
		free(tgt->faces);
		tgt->faces = next;
	}

}


static void _compute_normal(EZ_Tri_t* tri, EZ_Vec_t* dest) {

	EZ_Vec_t v1, v2, normal;

	EZ_Vec_t* p1 = &(tri->vert[0].pos);
	EZ_Vec_t* p2 = &(tri->vert[1].pos);
	EZ_Vec_t* p3 = &(tri->vert[2].pos);

	EZ_vec_sub(&v1, p3, p1);
	EZ_vec_sub(&v2, p2, p1);

	EZ_vec_cross(&normal, &v1, &v2);
	EZ_vec_normal(dest, &normal);
}


static void _transform_tri(EZ_Tri_t* tri, EZ_Mat4_t* trns) {

	int i;
	for (i = 0; i < 3; i++) {
		EZ_mat4_vmul( &(tri->vert[i].pos), trns, &(tri->vert[i].pos) );
	}
}




void EZ_draw3D_freeTarget(EZ_3DTarget_t* tgt) {
	if (tgt == NULL) return;

	_free_tri_list(tgt);
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

	_free_tri_list(tgt);

}

void EZ_draw3D_endScene(EZ_3DTarget_t* tgt) {

	EZ_Tri_list* node;


	/* clipping */
	/**/

	/* transoform and compute normals */
	for (node = tgt->faces; node != NULL; node = node->next) {

		_compute_normal(&node->tri, &node->tri.world_normal);
		_transform_tri(&node->tri, tgt->trns);
		_compute_normal(&node->tri, &node->tri.cam_normal);
	}

	/* sort */
	/**/

	/* render */
	for (node = tgt->faces; node != NULL; node = node->next) { 
		EZ_draw3D_renderTri(tgt, &node->tri);
		
	}
}



void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns) {
	
	int i;
	for (i = 0; i < mesh->f_count; i++)
		EZ_draw3D_tri(tgt, &mesh->faces[i], trns);

}

void EZ_draw3D_tri(EZ_3DTarget_t* tgt, EZ_Tri_t* tri, EZ_Mat4_t* trns) {

	/* add triangle to list */
	EZ_Tri_list *new = malloc( sizeof(EZ_Tri_list) );

	new->next = tgt->faces;
	new->tri = *tri;
	_transform_tri(&new->tri, trns);

	tgt->faces = new;
}



void EZ_draw3D_renderTri(EZ_3DTarget_t* tgt, EZ_Tri_t* tri) {



	/* project to 2D */
	int i;
	for (i = 0; i < 3; i++) {

		/* "z clipping" */
		if (tri->vert[i].pos.z < 0.0) return;

		/* apply projection matrix */
		EZ_mat4_vmul(&tri->vert[i].pos, tgt->proj, &tri->vert[i].pos );

		/* non linear part of the projection */
		const float z = 1.0 / tri->vert[i].pos.w;

		EZ_vec_scale(&tri->vert[i].pos, &tri->vert[i].pos, z);

		/* convert to pixel space */
		/* from -1 to +1 with y from bottom to top */
		tri->vert[i].sx = (tri->vert[i].pos.x + 1.0f) * 0.5f * tgt->img->w;
		tri->vert[i].sy = (tri->vert[i].pos.y + 1.0f) * 0.5f * tgt->img->h;
		tri->vert[i].uv.z  = z;

		if (tgt->do_uv_correction) {
			tri->vert[i].uv.x *= z;
			tri->vert[i].uv.y *= z;
		}
	}




	/* culling -  projected normal z componant  */
	if ( (tri->vert[1].pos.x - tri->vert[0].pos.x) * (tri->vert[2].pos.y - tri->vert[0].pos.y)
	   - (tri->vert[1].pos.y - tri->vert[0].pos.y) * (tri->vert[2].pos.x - tri->vert[0].pos.x)
	   > 0.0f) 
	return;


	/* light and shading */
	EZ_Vec_t l_dir = {{0.0f, 0.0f, 1.0f}};
	tri->illum = CLAMP(EZ_vec_dot(&tri->world_normal, &l_dir), 0.0, 1.0);



	/* raster */
	{

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



		EZ_3DRenderParam_t p;
		p.tri = tri;



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

		/* vertical "clipping" */
		int y_start = CLAMP(Y0, 0, HEIGHT);
		int y_mid   = CLAMP(Y1, 0, HEIGHT);
		int y_end   = CLAMP(Y2, 0, HEIGHT);

		float x_start = X0 + (y_start - Y0) * dxdy1;
		float x_end   = X0 + (y_start - Y0) * dxdy2;

		float u_start = U0 + (y_start - Y0) * dudy1;
		float u_end   = U0 + (y_start - Y0) * dudy2;
				
		float v_start = V0 + (y_start - Y0) * dvdy1;
		float v_end   = V0 + (y_start - Y0) * dvdy2;

		float z_start = Z0 + (y_start - Y0) * dzdy1;
		float z_end   = Z0 + (y_start - Y0) * dzdy2;


		int half = 0;

		/* scan lines */
		for (p.y = y_start; p.y < y_end; p.y++) {

			/* switch to bottom half of the triangle */
			if (half == 0 && p.y >= y_mid) {
				
				half = 1;

				dxdy1 = (float)(X2 - X1)/(Y2 - Y1);
				dudy1 = (float)(U2 - U1)/(Y2 - Y1);
				dvdy1 = (float)(V2 - V1)/(Y2 - Y1);
				dzdy1 = (float)(Z2 - Z1)/(Y2 - Y1);

				x_start = X1 + (p.y - Y1) * dxdy1; 
				x_end   = X0 + (p.y - Y0) * dxdy2;

				u_start = U1 + (p.y - Y1) * dudy1; 
				u_end   = U0 + (p.y - Y0) * dudy2;

				v_start = V1 + (p.y - Y1) * dvdy1;
				v_end   = V0 + (p.y - Y0) * dvdy2;

				z_start = Z1 + (p.y - Y1) * dzdy1;
				z_end   = Z0 + (p.y - Y0) * dzdy2;

			}

			/* interpolate uv along x */
			float dudx = (float)(u_end - u_start)/(x_end - x_start);
			float dvdx = (float)(v_end - v_start)/(x_end - x_start);
			float dzdx = (float)(z_end - z_start)/(x_end - x_start);


			/* sort from left to right */
			/* horizontal "clipping"   */
			int x_left, x_right;
			float u, v, z;

			if (x_start < x_end) {

				x_left  = CLAMP(x_start, 0, WIDTH);
				x_right = CLAMP(x_end,   0, WIDTH);
			
				u = u_start + (x_left - x_start) * dudx;
				v = v_start + (x_left - x_start) * dvdx;
				z = z_start + (x_left - x_start) * dzdx;
			}
			else {

				x_left  = CLAMP(x_end,   0, WIDTH);
				x_right = CLAMP(x_start, 0, WIDTH);
			
				u = u_end + (x_left - x_end) * dudx;
				v = v_end + (x_left - x_end) * dvdx;
				z = z_end + (x_left - x_end) * dzdx;
			}


			/* draw line */
			for (p.x = x_left; p.x < x_right; p.x++) {

				p.zloc = tgt->zbuff   + p.x + p.y * WIDTH;
				p.px   = tgt->img->px + p.x + p.y * WIDTH;

				if (tgt->do_uv_correction) {
					p.u = u/z; p.v = v/z; p.z = z;
				}
				else {
					p.u = u; p.v = v; p.z = z;
				}

				/* call shader */
				if (p.tri->mat->shad == NULL) {
					// EZ_shader_flat(&p);
				}
				else {
					p.tri->mat->shad(&p);
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




}


