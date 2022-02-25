#ifndef _EZGFX_SHADER_DEF_H_
#define _EZGFX_SHADER_DEF_H_

/*
   triangle defined with a shader (in material), 
   but shader is defined with a triangle (in render param)
*/

typedef struct __EZ_3DRenderParam_t__ EZ_3DRenderParam_t;

typedef void (EZ_Shader_t)(EZ_3DRenderParam_t* p);

#endif /* ezGfx_shader_def_h */