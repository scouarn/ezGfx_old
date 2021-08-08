#include "ezGfx_core.h"
#include "ezGfx_matrix.h"

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


typedef struct {
  EZ_mat pos[3];
  EZ_mat uv [3];
  EZ_px col;

} EZ_vertex;

typedef struct {
  EZ_vertex vertices[3];
  EZ_mat normal[3];
  EZ_Image* texture;
  EZ_px col;

} EZ_poly;


void EZ_draw3D_wireframe(EZ_Image target, EZ_poly mesh[], int nPoly, EZ_mat projection[], EZ_mat transform[]);

void EZ_draw3D_projTransform(EZ_mat matrix[], float fov, float aRatio, float zNear, float zFar);

void EZ_draw3D_unitCube(EZ_poly mesh[], EZ_px col);



#endif
