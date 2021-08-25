
/*
/
/	EZGFX 2D EXTENSION
/
/
/ SCOUARN, 2021
/
/	2D primitives rasterization, image manipulation, font rendering.
/
/
/
*/

#ifndef EZ_DRAW2D
#define EZ_DRAW2D

#include "ezGfx_core.h"
#include "ezGfx_matrix.h"


void EZ_draw2D_setBlendMode(enum EZ_BlendMode mode); //set the blendmode used in all the extension

//interact with the builtin translation vector
void EZ_draw2D_translate(int x, int y);
void EZ_draw2D_setTranslate(int x, int y);
void EZ_draw2D_getTranslate(int* x, int* y);



//print single char with given background and foreground color
void EZ_draw2D_printChar(EZ_Image target, uint32_t c, const EZ_Font font, EZ_Px fg, EZ_Px bg, int x, int y);

//print a string, handle control characters,
//wraps in a given with and height (in chars)
//return pointer to the char where it stopped (so to '\0' if it went to the end)
const char*	EZ_draw2D_printStr(EZ_Image target, const char* str, const EZ_Font font, EZ_Px fg, EZ_Px bg, int x, int y, int w_chars, int h_chars);


void EZ_draw2D_clear(EZ_Image target, EZ_Px col); //clear with uniform color
void EZ_draw2D_pixel(EZ_Image target, EZ_Px col, int x, int y); //draw single pixel
void EZ_draw2D_line (EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2); //line from point 1 to point 2

void EZ_draw2D_rect    (EZ_Image target, EZ_Px col, int x, int y, int w, int h); //rectangle with top left corner in x,y
void EZ_draw2D_fillRect(EZ_Image target, EZ_Px col, int x, int y, int w, int h);

void EZ_draw2D_tri     (EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2, int x3, int y3); //triangle with specified vertices
void EZ_draw2D_fillTri (EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2, int x3, int y3);

void EZ_draw2D_ellipse    (EZ_Image target, EZ_Px col, int x, int y, int semi_a, int semi_b); //ellipse with center x,y and semi axis
void EZ_draw2D_fillEllipse(EZ_Image target, EZ_Px col, int x, int y, int semi_a, int semi_b);

void EZ_draw2D_image           (EZ_Image target, const EZ_Image source, int x, int y); 							   //image copy with top left corner in x,y
void EZ_draw2D_croppedImage    (EZ_Image target, const EZ_Image source, int x, int y, int u, int v, int w, int h); //destination x,y and crop from u,v to u+w,v+h
void EZ_draw2D_resizedImage    (EZ_Image target, const EZ_Image source, int x, int y, int w, int h); 			   //resize to given size, skip or duplicate pixels when needed
void EZ_draw2D_transformedImage(EZ_Image target, const EZ_Image source, const mat3x3* transformation);     	       //draw image with given affine transformation matrix




#endif
