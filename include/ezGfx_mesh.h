#ifndef _EZGFX_MESH_H_
#define _EZGFX_MESH_H_

#include "ezGfx_pixel.h"
#include "ezGfx_image.h"
#include "ezGfx_vec.h"

typedef struct {
  EZ_Vec_t pos[3];
  struct {float u, v;} uv[3];
  struct {int x, y; float z;} screen[3];

  EZ_Vec_t normal;
  EZ_Px_t  col;
  
} EZ_Tri_t;


typedef struct {
  EZ_Tri_t* triangles;
  unsigned long nPoly;

  EZ_Image_t* texture;

} EZ_Mesh_t;


EZ_Mesh_t* EZ_mesh_unitCube();
void EZ_mesh_free(EZ_Mesh_t* mesh);

EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname);
void       EZ_mesh_saveOBJ(EZ_Mesh_t* mesh, const char* fname);


#endif /* ezGfx_mesh_h */