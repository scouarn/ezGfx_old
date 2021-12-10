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


/* do the non linear operation of the projection */
static void _mulwdiv(EZ_Vec_t* C, EZ_Mat4_t* A, EZ_Vec_t* B) {

	double w = A->v[3][0] * B->x
	         + A->v[3][1] * B->y
	         + A->v[3][2] * B->z
	         + A->v[3][3];

	EZ_Vec_t proj;
	EZ_mat4_vdown(&proj, A, B);
	
	/* don't divide by 0 */
	if (w != 0.0) EZ_vec_scale(C, &proj, 1.0 / w);
}


#define PROJECT_SUCCESS 0
#define PROJECT_BEHIND  1
#define PROJECT_AWAY    2

static int _project(EZ_Tri_t* tri, EZ_Mat4_t* proj, EZ_Mat4_t* trns, int width, int height) {

	int i;
	EZ_Vec_t transformed[3];
	EZ_Vec_t projected[3];

	/* transform  -  apply transform matrix */
	for (i = 0; i < 3; i++)
		EZ_mat4_vdown(&transformed[i], trns, &(tri->pos[i]) );


	/* compute world space normal */
	_normal(&tri->normal, &transformed[0], &transformed[1], &transformed[2]);

	/* don't render faces that are behind the camera */
	if (transformed[0].z < 0.0f || 
		transformed[1].z < 0.0f || 
		transformed[2].z < 0.0f) 

		return PROJECT_BEHIND;


	/* project to 2D  - apply projection matrix */
	for (i = 0; i < 3; i++)
		_mulwdiv(&projected[i], proj, &transformed[i]);


	/* culling */
	/* accurate according to the given perspective matrix */
	/* projected normal z componant  */
	/* allow to use other matrices */
	const float nz = (projected[1].x - projected[0].x) * (projected[2].y - projected[0].y) -
				     (projected[1].y - projected[0].y) * (projected[2].x - projected[0].x);

	if (nz > 0.0f) return PROJECT_AWAY;
	

	/* convert to pixel space */
	/* from -1 to +1 with y from bottom to top */
	for (i = 0; i < 3; i++) {
		tri->screen[i].x = (projected[i].x + 1.0f) * 0.5f * width;
		tri->screen[i].y = (projected[i].y + 1.0f) * 0.5f * height;
		tri->screen[i].z =  projected[i].z;
	}


	return PROJECT_SUCCESS;


}




void EZ_draw3D_tri(EZ_3DTarget_t* tgt, EZ_Tri_t* tri, EZ_Mat4_t* trns, EZ_3DMode_t mode) { 

	/* project the triangle */
	EZ_Mat4_t fullTrans;
	EZ_mat4_mul(&fullTrans, tgt->trns, trns);

	int result = _project(tri, tgt->proj, &fullTrans, tgt->img->w, tgt->img->h);
	if (result != PROJECT_SUCCESS) return;



	EZ_Px_t shade;

	switch (mode) {

		case EZ_3D_WIRE : shade = EZ_WHITE; break;
		case EZ_3D_FLAT : shade = tri->col; break;

		case EZ_3D_FLAT_SHADED : {

			/* light and shading */
			EZ_Vec_t l_dir = {{0.0f, 0.0f, 1.0f}};
			float b  = CLAMP(EZ_vec_dot(&tri->normal, &l_dir), 0.25f, 1.0f);

			shade = tri->col;
			shade.r *= b;
			shade.g *= b;
			shade.b *= b;
		}
		break;

		default : shade = EZ_MAGENTA; break;
	}


	/* draw the triangle */

	#define TS0 tri->screen[0]
	#define TS1 tri->screen[1]
	#define TS2 tri->screen[2]
	#define TPX tgt->img->px
	#define TIW tgt->img->w
	#define TIH tgt->img->h

	/* sort points (0_y : top, 1_y : mid, 2_y : bot) */
	if (TS0.y > TS1.y) {
		SWAP(TS0, TS1);
	}
	if (TS0.y > TS2.y) {
		SWAP(TS0, TS2);
	}
	if (TS1.y > TS2.y) {
		SWAP(TS1, TS2);
	}


	/* compute slopes */
	int dyTop = TS1.y - TS0.y;
	int dxTop = TS1.x - TS0.x;

	int dyMid = TS2.y - TS0.y;
	int dxMid = TS2.x - TS0.x;

	int dyBot = TS2.y - TS1.y;
	int dxBot = TS2.x - TS1.x;

	float slopeTop = (float)dxTop/dyTop;
	float slopeMid = (float)dxMid/dyMid;
	float slopeBot = (float)dxBot/dyBot;


	/* "vertical" clipping */
	int y0 = CLAMP(TS0.y, 0, TIH);
	int y1 = CLAMP(TS1.y, 0, TIH);
	int y2 = CLAMP(TS2.y, 0, TIH);


	int sx, sy;
	/*float tx, ty;*/


	/* get values of x for left and right line at y0 */
	float xTop = (TS0.x - TS0.y * slopeTop) + y0 * slopeTop;
	float xMid = (TS0.x - TS0.y * slopeMid) + y0 * slopeMid;


	/* scan lines */
	for (sy = y0; sy < y2; sy++) {

		/* this line actually avoids dividing by zero,
		   by skipping the top triangle when y1 == y2 (same for y2 == y3) */
		if (sy == y1) {
			slopeTop = slopeBot;
			xMid = (TS0.x - TS0.y * slopeMid) + y1 * slopeMid;
			xTop = (TS1.x - TS1.y * slopeBot) + y1 * slopeBot;	
		}

		/* "horizontal" clipping*/
		int xLeft  = CLAMP( MIN(xTop, xMid), 0, TIW);
		int xRight = CLAMP( MAX(xTop, xMid), 0, TIW);

		switch (mode) {

			case EZ_3D_WIRE :
				TPX[xLeft + sy * TIW] = shade;
				TPX[xLeft + sy * TIW] = shade;
			break;


			case EZ_3D_FLAT :
			case EZ_3D_FLAT_SHADED :

				for (sx = xLeft; sx < xRight; sx++)
					TPX[sx + sy * TIW] = shade;

			break;

			case EZ_3D_TEXTURE :
				for (sx = xLeft; sx < xRight; sx++)
					TPX[sx + sy * TIW] = shade;
			break;


		}


		xTop += slopeTop; xMid += slopeMid;
	}




}

void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns, EZ_3DMode_t mode) {
	int i;

	/* draw each triangle */
	for (i = 0; i < mesh->nPoly; i++) {
		EZ_draw3D_tri(tgt, mesh->triangles + i, trns, mode);
	}

}
