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


#include "ezGfx_core.h"
#include "ezGfx_matrix.h"
#include "ezGfx_utils.h"

typedef struct {
  vec3f pos;
  vec3f uv;
  EZ_Px_t col;

} EZ_Vertex_t;

typedef struct {
  EZ_Vertex_t points[3];
  vec3f norm;
  EZ_Px_t col;

} EZ_Tri_t;


typedef struct {
  EZ_Tri_t* triangles;
  unsigned long nPoly;

  EZ_Image_t* texture;

/*  EZ_Image_t* bumpmap;
 *  ...maps maps maps
 *  everything map */

} EZ_Mesh_t;


void EZ_draw3D_drawMesh(EZ_Image_t* target, EZ_Mesh_t* mesh, const mat4x4* proj, const mat4x4* transform);
void EZ_draw3D_tri     (EZ_Image_t* target, EZ_Tri_t* tri,   const mat4x4* proj, const mat4x4* transform);

EZ_Mesh_t* EZ_draw3D_unitCube();
void EZ_draw3D_freeMesh(EZ_Mesh_t* mesh);



#endif /* ezGfx_draw3D_h */