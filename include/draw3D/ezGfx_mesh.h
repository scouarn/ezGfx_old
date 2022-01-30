#ifndef _EZGFX_MESH_H_
#define _EZGFX_MESH_H_

#include <stdbool.h>

#include "ezGfx_vec.h"
#include "ezGfx_mat4.h"

#include "ezGfx_pixel.h"
#include "ezGfx_image.h"

#include "ezGfx_shader_def.h"


#define MESH_MAT_COUNT 16

typedef struct {

	EZ_Shader_t* shad;
	EZ_Image_t* tex;
	EZ_Px_t col;

} EZ_Material_t;


typedef struct {
	EZ_Vec_t pos;
	EZ_Vec_t uv;
	EZ_Vec_t normal;
	EZ_Px_t col;
	
	int sx, sy; /* projected screen position */

} EZ_Vertex_t;


typedef struct EZ_Tri_t {
	
	EZ_Vertex_t vert[3];
	EZ_Material_t* mat;
	float illum;

	EZ_Vec_t world_normal;
	EZ_Vec_t cam_normal;
	EZ_Vec_t screen_normal;

} EZ_Tri_t;


typedef struct {

	EZ_Tri_t* faces;
	EZ_Material_t materials[MESH_MAT_COUNT];

	int f_count;

} EZ_Mesh_t;


EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname);
void EZ_mesh_loadSingleTexture(EZ_Mesh_t* mesh, const char* fname);
void EZ_mesh_loadMTL(EZ_Mesh_t* mesh, const char* fname);

void EZ_mesh_free(EZ_Mesh_t* mesh);
void EZ_mesh_freeTextures(EZ_Mesh_t* mesh);

#endif /* ezGfx_mesh_h */