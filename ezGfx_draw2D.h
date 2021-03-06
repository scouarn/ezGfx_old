#include "ezGfx_core.h"

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

enum alphaMode {ALPHA_IGNORE, ALPHA_FAST, ALPHA_BLEND};
enum interMode {INTER_LINEAR};

void EZ_draw2D_alphaMode(enum alphaMode mode);
void EZ_draw2D_interMode(enum interMode mode);

void EZ_draw2D_clear(EZ_Image* target, EZ_px col);

void EZ_draw2D_pixel   (EZ_Image* target, EZ_px col, int x1, int y1);
void EZ_draw2D_line    (EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2);

void EZ_draw2D_rect    (EZ_Image* target, EZ_px col, int x1, int y1, int h,  int w);
void EZ_draw2D_fillRect(EZ_Image* target, EZ_px col, int x1, int y1, int h,  int w);

void EZ_draw2D_tri     (EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3);
void EZ_draw2D_fillTri (EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3);
void EZ_draw2D_texTri  (EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3,
                        EZ_Image* texture,           int u1, int v1, int u2, int v2, int u3, int v3);

void EZ_draw2D_elli    (EZ_Image* target, EZ_px col, int x1, int y1, int a,  int b);
void EZ_draw2D_fillElli(EZ_Image* target, EZ_px col, int x1, int y1, int a,  int b);

void EZ_draw2D_copyImage     (EZ_Image* target, EZ_Image* source, int x1, int y1);
void EZ_draw2D_croppedImage  (EZ_Image* target, EZ_Image* source, int x1, int y1, int u1, int v1, int u2, int v2);
void EZ_draw2D_resizedImage  (EZ_Image* target, EZ_Image* source, int x1, int y1, int w, int h);
void EZ_draw2D_transformImage(EZ_Image* target, EZ_Image* source, int x1, int y1, void* matrix);


#endif
