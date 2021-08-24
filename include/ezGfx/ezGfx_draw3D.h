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


#ifndef EZ_DRAW3D
#define EZ_DRAW3D

#include "ezGfx/ezGfx_core.h"
#include "ezGfx/ezGfx_matrix.h"
#include "ezGfx/ezGfx_utils.h"

typedef struct {
  vec3f pos;
  vec3f uv;
  EZ_Px col;

} EZ_Vertex;

typedef struct {
  EZ_Vertex points[3];
  vec3f norm;
  EZ_Px col;

} EZ_Tri;


typedef struct {
  EZ_Tri* triangles;
  uint32_t nPoly;

  EZ_Image* texture;

//  EZ_Image* bumpmap;
//  ...maps maps maps
//  everything map

} EZ_Mesh;


void EZ_draw3D_drawMesh(EZ_Image target, const EZ_Mesh, const mat4x4* projection, const mat4x4* transform);
void EZ_draw3D_tri(EZ_Image target, const EZ_Tri*, const mat4x4* projection, const mat4x4* transform);



EZ_Mesh EZ_draw3D_unitCube();

void EZ_draw3D_freeMesh(EZ_Mesh);



#endif
