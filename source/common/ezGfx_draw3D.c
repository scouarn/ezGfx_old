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




#define PROJECT_SUCCESS 0
#define PROJECT_BEHIND  1
#define PROJECT_AWAY    2
#define PROJECT_DIVZERO 3




void EZ_draw3D_tri(EZ_3DTarget_t* tgt, EZ_Image_t* texture, EZ_Tri_t* tri, EZ_Mat4_t* trns, EZ_3DMode_t mode) { 

	#define PX tgt->img->px
	#define WIDTH tgt->img->w
	#define HEIGHT tgt->img->h

	int i;

	EZ_Mat4_t fullTrans;
	EZ_Vec_t transformed[3], projected[3];
	
	struct { 
		int x, y;
		float u, v, z;
	} res[3];


	/* project the triangle */
	EZ_mat4_mul(&fullTrans, tgt->trns, trns);


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
		if (projected[i].w == 0.0) 
			return;


		const float z = 1.0 / projected[i].w;

		EZ_vec_scale(&projected[i], &projected[i], z);


		/* convert to pixel space */
		/* from -1 to +1 with y from bottom to top */
		res[i].x = (projected[i].x + 1.0f) * 0.5f * WIDTH;
		res[i].y = (projected[i].y + 1.0f) * 0.5f * HEIGHT;

		/* write uv information */
		res[i].u = tri->vert[i].u * z;
		res[i].v = tri->vert[i].v * z;
		res[i].z = z;
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
	float shade = CLAMP(EZ_vec_dot(&tri->normal, &l_dir), 0.25f, 1.0f);



	/* sort points (0_y : top, 1_y : mid, 2_y : bot) */
	if (res[0].y > res[1].y) {
		SWAP(res[0],  res[1]);
	}
	if (res[0].y > res[2].y) {
		SWAP(res[0],  res[2]);
	}
	if (res[1].y > res[2].y) {
		SWAP(res[1],  res[2]);
	}

	/* compute slopes */
	int dx1 = res[1].x - res[0].x;
	int dy1 = res[1].y - res[0].y;

	int dx2 = res[2].x - res[0].x;
	int dy2 = res[2].y - res[0].y;

	int dx3 = res[2].x - res[1].x;
	int dy3 = res[2].y - res[1].y;

	float dx_start = (float)dx1/dy1;
	float dx_end   = (float)dx2/dy2;


	/* "vertical" clipping */
	int y_start = CLAMP(res[0].y, 0, HEIGHT);
	int y_end   = CLAMP(res[2].y, 0, HEIGHT);

	/* get values of x for left and right line at y0 */
	float x_start = res[0].x + (y_start - res[0].y) * dx_start;
	float x_end   = res[0].x + (y_start - res[0].y) * dx_end;

	int sx, sy;



	/* scan lines */
	for (sy = y_start; sy < y_end; sy++) {

		/* this line actually avoids dividing by zero,
		   by skipping the top triangle when y1 == y2 (same for y2 == y3) */
		if (sy == res[1].y) {
			dx_start = (float)dx3/dy3;
			x_start = res[1].x; /* seems to glitch if not corrected */
		}


		/* "horizontal" clipping*/
		int xLeft  = CLAMP(x_start, 0, WIDTH);
		int xRight = CLAMP(x_end,   0, WIDTH);

		/* left-right sorting */
		if (xLeft > xRight)
			SWAP(xLeft, xRight);


		switch (mode) {

			case EZ_3D_WIRE :
				PX[xLeft + sy * WIDTH] = EZ_WHITE;
				PX[xLeft + sy * WIDTH] = EZ_WHITE;
			break;


			case EZ_3D_FLAT :
				for (sx = xLeft; sx < xRight; sx++)
					PX[sx + sy * WIDTH] = tri->col;
			break;


			case EZ_3D_FLAT_SHADED : {

				EZ_Px_t col = tri->col;
				col.r *= shade;
				col.g *= shade;
				col.b *= shade;

				for (sx = xLeft; sx < xRight; sx++)
					PX[sx + sy * WIDTH] = col;
			}
			break;

			case EZ_3D_TEXTURE : {
					
				for (sx = xLeft; sx < xRight; sx++) {

					PX[sx + sy * WIDTH] = texture->px[0 + 0 * texture->w];
				}

			}
			break;


			default : break;


		}

		x_start += dx_start; 
		x_end   += dx_end;
	}



}

void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns, EZ_3DMode_t mode) {
	int i;

	/* draw each triangle */
	for (i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_tri(tgt, mesh->texture, mesh->triangles + i, trns, mode);
	}

}
