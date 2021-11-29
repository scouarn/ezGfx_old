#ifndef _EZGFX_IMAGE_H_
#define _EZGFX_IMAGE_H_

#include "ezGfx_pixel.h"

typedef struct {
	unsigned int w, h; /* size in pixels */
	EZ_Px_t* px;       /* pixel array */

} EZ_Image_t;


EZ_Image_t* EZ_createImage(int w, int h); /* allocates memory for an image of given size */
void        EZ_freeImage(EZ_Image_t*);    /* free allocated memory */

EZ_Image_t* EZ_load_BMP(const char* fname);
void        EZ_save_BMP(EZ_Image_t* image, const char* fname);

#endif /* ezGfx_image_h */
