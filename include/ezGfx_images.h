#include "ezGfx_core.h"

/*
/
/	EZGFX 2D EXTENSION
/
/
/ SCOUARN, 2021
/
/	Image loading, saving and transformation.
/
/
/
*/

#ifndef EZ_IMAGES
#define EZ_IMAGES


EZ_Image* EZ_images_load(const char* fName);
void  	  EZ_images_save(EZ_Image* image, const char* fName);

void EZ_images_transform(EZ_Image* target, EZ_Image* source, void* matrix);
//3D models, json parsing, etc...

#endif
