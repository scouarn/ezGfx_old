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

#include "ezGfx_matrix.h"
#include "ezGfx_mesh.h"

void EZ_draw3D_drawMesh(EZ_Image_t* target, EZ_Mesh_t* mesh, const mat4x4* proj, const mat4x4* transform);
void EZ_draw3D_tri     (EZ_Image_t* target, EZ_Tri_t* tri,   const mat4x4* proj, const mat4x4* transform);


#endif /* ezGfx_draw3D_h */