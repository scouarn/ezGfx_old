#ifndef _EZGFX_FILES_H_
#define _EZGFX_FILES_H_


#include "ezGfx_core.h"

/* Images */
EZ_Image_t* EZ_load_BMP(const char* fname);
void        EZ_save_BMP(EZ_Image_t* image, const char* fname);

/* Fonts */
EZ_Font_t* EZ_load_PSF2(const char* fname);
void       EZ_save_PSF2(EZ_Font_t* font, const char* fname);


/* OBJ 3D meshes */
#ifdef _EZGFX_DRAW3D_H
EZ_Mesh_t* EZ_load_OBJ(const char* fname);
void    EZ_save_OBJ(EZ_Mesh_t* mesh, const char* fname);
#endif


#endif /* ezGfx_files_h */