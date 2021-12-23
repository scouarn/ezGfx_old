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
#include <stdbool.h>


typedef void (EZ_Shader_t)(EZ_Px_t* px, EZ_Image_t* tex, EZ_Px_t col, float illum, float u, float v, float z);

/* default shaders */
EZ_Shader_t EZ_draw3D_textureShader;
EZ_Shader_t EZ_draw3D_flatShader;

typedef struct {
	EZ_Image_t* img;
	EZ_Mat4_t* proj;
	EZ_Mat4_t* trns;
	float* zbuff;

	EZ_Shader_t* shader;
	bool do_uv_correction;

} EZ_3DTarget_t;


EZ_3DTarget_t* EZ_draw3D_makeTarget(EZ_Image_t* img, EZ_Mat4_t* proj, EZ_Mat4_t* trns);
void           EZ_draw3D_freeTarget(EZ_3DTarget_t* tgt);

void EZ_draw3D_startScene(EZ_3DTarget_t* tgt);
void EZ_draw3D_endScene  (EZ_3DTarget_t* tgt);

void EZ_draw3D_tri (EZ_3DTarget_t* tgt, EZ_Image_t* tex, EZ_Tri_t* tri, EZ_Mat4_t* trns);
void EZ_draw3D_mesh(EZ_3DTarget_t* tgt, EZ_Mesh_t* mesh, EZ_Mat4_t* trns);


#endif /* ezGfx_draw3D_h */