/*
/
/	EZGFX 3D EXTENSION
/
/
/ SCOUARN, 2021
/
/	3D primitives, textured models.
/
/
/
*/


#ifndef _EZGFX_DRAW3D_H_
#define _EZGFX_DRAW3D_H_

#include "ezGfx_vec.h"
#include "ezGfx_mat4.h"
#include "ezGfx_mesh.h"
#include "ezGfx_shader.h"

#include <stdbool.h>

#define EZ_DRAW3D_MODE_FLAT     0
#define EZ_DRAW3D_MODE_TEXTURED 1


typedef struct {
	EZ_Image_t* img; /* target image */
	EZ_Mat4_t* proj; /* camera projection */
	EZ_Mat4_t* trns; /* world transform */

	float* zbuff;    /* z buffer */
	EZ_Tri_t* faces; /* list of triangles to be rendered */

 	/* render parameters */
	unsigned int do_uv_correction : 1; /* default true */
	unsigned int do_tri_sorting   : 1; /* default false */

} EZ_3DTarget_t;


EZ_3DTarget_t* EZ_draw3D_makeTarget(EZ_Image_t* img, EZ_Mat4_t* proj, EZ_Mat4_t* trns);
void           EZ_draw3D_freeTarget(EZ_3DTarget_t* tgt);


void EZ_draw3D_startScene(EZ_3DTarget_t* tgt);
void EZ_draw3D_endScene  (EZ_3DTarget_t* tgt);

void EZ_draw3D_tri (EZ_3DTarget_t* tgt, EZ_Tri_t* tri,   EZ_Mat4_t* trns);
void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns);


#endif /* ezGfx_draw3D_h */