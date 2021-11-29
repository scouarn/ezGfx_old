#ifndef _EZGFX_FILES_FONT_H_
#define _EZGFX_FILES_FONT_H_

#include <stdint.h>

#include "ezGfx_pixel.h"
#include "ezGfx_image.h"


typedef struct {
	int w, h;
	int wb; /* number of bytes in each row */
	uint8_t** data;   /* matrix of pixel value : data->char->line */

} EZ_Font_t;



EZ_Font_t* EZ_createFont(int w, int h); /* allocates memory for 256 chars with given size in pixels */
void       EZ_freeFont(EZ_Font_t* font);

EZ_Font_t* EZ_load_PSF2(const char* fname);
void       EZ_save_PSF2(EZ_Font_t* font, const char* fname);


/*void EZ_font_setStyle(EZ_BlendMode_t mode);*/

/* print single char with given background and foreground color */
void EZ_draw2D_printChar(EZ_Image_t* target, unsigned int c, EZ_Font_t* font, EZ_Px_t fg, EZ_Px_t bg, int x, int y);

/* print a string, handle control characters,
 * wraps in a given with and height (in chars)
 * return pointer to the char where it stopped (so to '\0' if it went to the end) */
const char*	EZ_draw2D_printStr(EZ_Image_t* target, const char* str, EZ_Font_t* font, EZ_Px_t fg, EZ_Px_t bg, int x, int y, int w_chars, int h_chars);



#endif /* ezGfx_files_font_h */