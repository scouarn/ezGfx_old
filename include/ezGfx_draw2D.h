
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

#ifndef _EZGFX_DRAW2D_H_
#define _EZGFX_DRAW2D_H_


#include "ezGfx_image.h"
#include "ezGfx_pixel.h"
#include "ezGfx_utils.h"
#include "ezGfx_mat3.h"
#include "ezGfx_vec.h"


void EZ_draw2D_setBlendMode(EZ_BlendMode_t mode);

void EZ_draw2D_clear(EZ_Image_t* target, EZ_Px_t col); /* clear with uniform color */
void EZ_draw2D_pixel(EZ_Image_t* target, EZ_Px_t col, int x, int y); /* draw single pixel */
void EZ_draw2D_line (EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2); /* line from point 1 to point 2 */

void EZ_draw2D_rect    (EZ_Image_t* target, EZ_Px_t col, int x, int y, int w, int h); /* rectangle with top left corner in x,y */
void EZ_draw2D_fillRect(EZ_Image_t* target, EZ_Px_t col, int x, int y, int w, int h);

void EZ_draw2D_tri     (EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2, int x3, int y3); /* triangle with specified vertices */
void EZ_draw2D_fillTri (EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2, int x3, int y3);

void EZ_draw2D_ellipse    (EZ_Image_t* target, EZ_Px_t col, int x, int y, int semi_a, int semi_b); /* ellipse with center x,y and semi axis */
void EZ_draw2D_fillEllipse(EZ_Image_t* target, EZ_Px_t col, int x, int y, int semi_a, int semi_b);

void EZ_draw2D_image           (EZ_Image_t* target, EZ_Image_t* source, int x, int y); 							   /* image copy with top left corner in x,y */
void EZ_draw2D_croppedImage    (EZ_Image_t* target, EZ_Image_t* source, int x, int y, int u, int v, int w, int h); /* destination x,y and crop from u,v to u+w,v+h */
void EZ_draw2D_resizedImage    (EZ_Image_t* target, EZ_Image_t* source, int x, int y, int w, int h); 			   /* resize to given size, skip or duplicate pixels when needed */
void EZ_draw2D_transformedImage(EZ_Image_t* target, EZ_Image_t* source, EZ_Mat3_t* transformation); /* takes a 3x3 matrix */     	       /* draw image with given affine transformation matrix */


#endif /* ezGfx_draw2D_h */