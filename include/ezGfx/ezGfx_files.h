

#ifndef EZGFX_FILES
#define EZGFX_FILES

#include "ezGfx/ezGfx_core.h"


//images
EZ_Image EZ_load_BMP(const char* fname);
void  	 EZ_save_BMP(const EZ_Image image, const char* fname);

EZ_Image EZ_load_PNG(const char* fname);
void  	 EZ_save_PNG(const EZ_Image image, const char* fname);



//fonts
//deprecated
// EZ_Font EZ_load_font(const char* fname);
// void    EZ_save_font(const EZ_Font font, const char* fname);

EZ_Font EZ_load_PSF2(const char* fname);
void    EZ_save_PSF2(const EZ_Font font, const char* fname);


//mesh
#ifdef EZ_DRAW3D
EZ_Mesh EZ_load_OBJ(const char* fname);
void    EZ_save_OBJ(const EZ_Mesh mesh, const char* fname);
#endif


//audio
#ifdef EZSFX_CORE


EZ_PCMArray EZ_load_WAV(const char* fname);
void		EZ_save_WAV(const EZ_PCMArray, const char* fname);

EZ_Note* EZ_load_MIDI(const char* fname);
void	 EZ_save_MIDI(const EZ_Note*, const char* fname);

#endif



#endif