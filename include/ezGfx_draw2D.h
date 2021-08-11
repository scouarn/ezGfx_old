
/*
/
/	EZGFX 2D EXTENSION
/
/
/ SCOUARN, 2021
/
/	2D primitives rasterization and image manipulation.
/
/
/
*/

#ifndef EZ_DRAW2D
#define EZ_DRAW2D

#include "ezGfx_core.h"
#include "matrix.h"

enum interMode {INTER_LINEAR, INTER_NEAREST};


typedef struct {
	int h_px;
	int w_px;
	int w_bytes;
	unsigned char** data;

} EZ_Font;


EZ_Image EZ_draw2D_loadBMP(const char* fname);
void  	 EZ_draw2D_saveBMP(EZ_Image image, const char* fname);

EZ_Font EZ_draw2D_makeFont(int w, int h);
EZ_Font EZ_draw2D_loadFont(const char* fname);
void    EZ_draw2D_saveFont(EZ_Font font, const char* fname);
void    EZ_draw2D_freeFont(EZ_Font font);


void	    EZ_draw2D_printChar(EZ_Image target, unsigned int c, EZ_Font font, EZ_px fg, EZ_px bg, int x, int y);
const char*	EZ_draw2D_printStr(EZ_Image target, const char* str, EZ_Font font, EZ_px fg, EZ_px bg, int x, int y, int w_chars, int h_chars);


void EZ_draw2D_alphaMode(enum EZ_blendMode mode);
void EZ_draw2D_interMode(enum interMode mode);

void EZ_draw2D_translate(int x, int y);
void EZ_draw2D_setTranslate(int x, int y);
void EZ_draw2D_getTranslate(int* x, int* y);


void EZ_draw2D_clear(EZ_Image target, EZ_px col);
void EZ_draw2D_pixel(EZ_Image target, EZ_px col, int x, int y);
void EZ_draw2D_line (EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2);

void EZ_draw2D_rect    (EZ_Image target, EZ_px col, int x, int y, int w, int h);
void EZ_draw2D_fillRect(EZ_Image target, EZ_px col, int x, int y, int w, int h);

void EZ_draw2D_tri     (EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3);
void EZ_draw2D_fillTri (EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3);

//TODO
void EZ_draw2D_texTri  (EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3,
						EZ_Image texture,           int u1, int v1, int u2, int v2, int u3, int v3);

void EZ_draw2D_ellipse    (EZ_Image target, EZ_px col, int x, int y, int semi_a, int semi_b);
void EZ_draw2D_fillEllipse(EZ_Image target, EZ_px col, int x, int y, int semi_a, int semi_b);

void EZ_draw2D_image         (EZ_Image target, EZ_Image source, int x, int y);
void EZ_draw2D_croppedImage  (EZ_Image target, EZ_Image source, int x, int y, int u, int v, int w, int h);
void EZ_draw2D_resizedImage  (EZ_Image target, EZ_Image source, int x, int y, int w, int h);

//TODO 
void EZ_draw2D_transformImage(EZ_Image target, EZ_Image source, mat3x3* transformation);




#endif
