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

void EZ_draw3D_drawMesh(EZ_Image_t* target, EZ_Mesh_t* mesh, EZ_Mat4_t* proj, EZ_Mat4_t* transform);
void EZ_draw3D_tri     (EZ_Image_t* target, EZ_Tri_t* tri,   EZ_Mat4_t* proj, EZ_Mat4_t* transform);


#endif /* ezGfx_draw3D_h */