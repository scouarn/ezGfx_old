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
#include <stdbool.h>


typedef struct {
	EZ_Image_t* img;
	EZ_Mat4_t* proj;
	EZ_Mat4_t* trns;
	float* zbuff;

} EZ_3DTarget_t;


typedef enum {
	EZ_3D_WIRE,
	EZ_3D_FLAT,
	EZ_3D_FLAT_SHADED,
	EZ_3D_TEXTURE
	
} EZ_3DMode_t;


EZ_3DTarget_t* EZ_draw3D_makeTarget(EZ_Image_t* img, EZ_Mat4_t* proj, EZ_Mat4_t* trns);
void           EZ_draw3D_freeTarget(EZ_3DTarget_t* tgt);

void EZ_draw3D_startScene(EZ_3DTarget_t* tgt); /* reset the depth buffer */
void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns, EZ_3DMode_t mode);
void EZ_draw3D_tri (EZ_3DTarget_t* tgt, EZ_Tri_t*   tri, EZ_Mat4_t* trns, EZ_3DMode_t mode);


#endif /* ezGfx_draw3D_h */