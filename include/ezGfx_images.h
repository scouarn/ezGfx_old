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


EZ_Image* EZ_images_load(const char* fname);
void  	  EZ_images_save(EZ_Image* image, const char* fname);

//TODO
//void EZ_images_transform(EZ_Image* target, EZ_Image* source, void* matrix);


#endif
