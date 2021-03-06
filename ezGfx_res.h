#include "ezGfx_core.h"

/*
/
/	EZGFX 2D EXTENSION
/
/
/ SCOUARN, 2021
/
/	For ressource loading and saving.
/
/
/
*/

#ifndef EZ_IMAGEFORMAT
#define EZ_IMAGEFORMAT


EZ_Image* EZ_loadImage(const char* fName);
void  	  EZ_saveImage(EZ_Image* image, const char* fName);


#endif
