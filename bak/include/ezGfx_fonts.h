/*
/
/	EZGFX FONT EXTENSION
/
/
/   SCOUARN, July 2021
/
/	Bitmap font renderer.
/
/
/
*/



#ifndef EZ_FONTS
#define EZ_FONTS

#include "ezGfx_core.h"


typedef struct {
	int h_px;
	int w_px;
	int w_bytes;
	unsigned char** data;

} EZ_Font;


EZ_Font EZ_fonts_make(int w, int h);
EZ_Font EZ_fonts_load(const char* fname);
void    EZ_fonts_save(EZ_Font font, const char* fname);
void    EZ_fonts_free(EZ_Font font);

void	EZ_fonts_printChar(EZ_Image target, unsigned int c, EZ_Font font, EZ_px fg, EZ_px bg, int x0, int y0);
const char*	EZ_fonts_printStr(EZ_Image target, const char* str, EZ_Font font, EZ_px fg, EZ_px bg, int x0, int y0, int w_chars, int h_chars);

#endif