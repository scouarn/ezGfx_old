#ifndef _EZGFX_MESH_H_
#define _EZGFX_MESH_H_

#include "ezGfx_pixel.h"
#include "ezGfx_image.h"
#include "ezGfx_vec.h"

typedef struct {
	EZ_Vec_t pos;
	EZ_Vec_t normal;
	EZ_Vec_t uv;
	int sx, sy;

	EZ_Px_t col;
	
} EZ_Vertex_t;

typedef struct EZ_Tri_t {
	EZ_Vertex_t vert[3];

	EZ_Vec_t normal;
	EZ_Px_t col;
	float illum;

	struct EZ_Tri_t* next;

} EZ_Tri_t;


typedef struct {
  EZ_Tri_t* triangles; /* LINKED LIST of triangles */
  unsigned long nPoly;
  EZ_Image_t* texture;

} EZ_Mesh_t;


void EZ_mesh_free(EZ_Mesh_t* mesh);

EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname);


#endif /* ezGfx_mesh_h */