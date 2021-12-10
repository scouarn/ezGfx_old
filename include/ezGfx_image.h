#ifndef _EZGFX_IMAGE_H_
#define _EZGFX_IMAGE_H_

#include "ezGfx_pixel.h"

typedef struct {
	unsigned int w, h; /* size in pixels */
	EZ_Px_t* px;       /* pixel array */

} EZ_Image_t;


EZ_Image_t* EZ_image_make(int w, int h); /* allocates memory for an image of given size */
void        EZ_image_free(EZ_Image_t*);    /* free allocated memory */

EZ_Image_t* EZ_image_loadBMP(const char* fname);
void        EZ_image_saveBMP(EZ_Image_t* image, const char* fname);

#endif /* ezGfx_image_h */
