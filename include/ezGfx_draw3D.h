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

#include "ezGfx_core.h"
#include "matrix.h"

typedef struct {
  vec3f pos;
  vec3f uv;
  EZ_Px col;

} EZ_Vertex;

typedef struct {
  EZ_Vertex points[3];
  vec3f norm;
  EZ_Px col;

} EZ_Poly;


typedef struct {
  EZ_Poly* triangles;
  u32 nPoly;

  EZ_Image* texture;

//  EZ_Image* bumpmap;
//  ...maps maps maps
//  everything map

} EZ_Mesh;


void EZ_draw3D_wireframe(EZ_Image target, const EZ_Mesh, const mat4x4* projection, const mat4x4* transform);
void EZ_draw3D_tri(EZ_Image target, const EZ_Poly, const mat4x4* projection, const mat4x4* transform);



EZ_Mesh EZ_draw3D_unitCube();

void EZ_draw3D_freeMesh(EZ_Mesh);



#endif
