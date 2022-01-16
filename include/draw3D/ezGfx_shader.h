#ifndef _EZGFX_SHADER_H_
#define _EZGFX_SHADER_H_

#include "ezGfx_mesh.h"
#include "ezGfx_shader_def.h"

struct __EZ_3DRenderParam_t__ {

	int x, y;      /* target coordinates */
	float u, v, z; /* uv coordinates */

	EZ_Px_t* px; /* pointer to pixel to be written */
	float* zloc; /* pointer to corresponding location in z buffer */

	EZ_Tri_t* tri; /* triangle being rendered */
	
};


/* default shaders */
EZ_Shader_t EZ_shader_textured;
EZ_Shader_t EZ_shader_flat;



#endif /* ezGfx_shader_h */