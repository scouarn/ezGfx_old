#ifndef _EZGFX_MESH_H_
#define _EZGFX_MESH_H_

#include "ezGfx_vec.h"
#include "ezGfx_mat4.h"

#include "ezGfx_pixel.h"
#include "ezGfx_image.h"

#include "ezGfx_shader_def.h"


typedef struct {
	EZ_Vec_t pos;
	EZ_Vec_t uv;
	EZ_Vec_t normal;
	EZ_Px_t col;

	int sx, sy; /* projected screen position */
	
} EZ_Vertex_t;


#define DEFAULT_MAT (EZ_Material_t){ NULL, NULL, EZ_WHITE };

typedef struct {

	EZ_Shader_t* shad;
	EZ_Image_t* tex;
	EZ_Px_t col;

} EZ_Material_t;


typedef struct EZ_Tri_t {
	
	EZ_Vertex_t vert[3];

	EZ_Material_t* mat;

	EZ_Vec_t normal;
	EZ_Vec_t cam_normal;
	float illum;

	struct EZ_Tri_t* next;

} EZ_Tri_t;


typedef struct {

	EZ_Tri_t* faces;	/* list */
	EZ_Material_t* materials;	/* array */

	int f_count, m_count;


} EZ_Mesh_t;


void EZ_mesh_free(EZ_Mesh_t* mesh);

EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname);
// EZ_Mesh_t* EZ_mesh_loadOFF(const char* fname);

// void EZ_mesh_EZOFFfree(EZ_Mesh_t* mesh); /* also free the materials and the materials's textures */
// EZ_Mesh_t* EZ_mesh_loadEZOFF(const char* fname);

#endif /* ezGfx_mesh_h */