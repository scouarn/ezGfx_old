#ifndef _EZGFX_CORE_COMMON_H_
#define _EZGFX_CORE_COMMON_H_
#else
#error _EZGFX_CORE_COMMON_H_ already defined, this is not a "true header".
#endif

/* Stuff that is not platform dependant but part of the core */

#include "ezGfx_core.h"
#include "ezGfx_utils.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Time */
static struct timespec startTime;
static struct timespec lastTime;
static volatile bool running;

/* Drawing */
static EZ_Image_t* canvas;
static int winWidth;
static int winHeight;

typedef struct { 
	int x1, y1, x2, y2, cx, cy;
} __rect__;

static void getCanvasRect(__rect__*);


/* Inputs */
static EZ_KeyCode_t keyMap(int scancode);
static EZ_Key_t keyStates[_numberOfKeys];
static EZ_Mouse_t mouseState;


/* Callbacks */
static void (*callback_init)(void);
static void (*callback_draw)(double);
static void (*callback_kill)(void);
static void (*callback_keyPressed)(EZ_Key_t*); 
static void (*callback_keyReleased)(EZ_Key_t*);
static void (*callback_mouse)(EZ_Mouse_t*); 


void EZ_setCallback_init( void (*f)(void) ) {
	callback_init = f;
}

void EZ_setCallback_draw( void (*f)(double dt) ) {
	callback_draw = f;
}

void EZ_setCallback_kill( void (*f)(void) ) {
	callback_kill = f;
}

void EZ_setCallback_keyPressed(  void (*f)(EZ_Key_t*) ) {
	callback_keyPressed = f;
}

void EZ_setCallback_keyReleased( void (*f)(EZ_Key_t*)) {
	callback_keyReleased = f;
}

void EZ_setCallback_mouse(  void (*f)(EZ_Mouse_t*) ) {
	callback_mouse = f;
}


double EZ_getTime() {
	double time;

	/* accurate time in seconds */
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	time = (now.tv_sec - startTime.tv_sec) + (now.tv_nsec - startTime.tv_nsec) / 1000000000.0;

	return time;
}

EZ_Key_t* EZ_getKey(EZ_KeyCode_t code) {
	return &keyStates[code];
}

EZ_Mouse_t* EZ_getMouse() {
	return &mouseState;
}


EZ_Image_t* EZ_createImage(int w, int h) {

	EZ_Image_t* img = malloc( sizeof(EZ_Image_t) );
	
	img->w = w;
	img->h = h;
	img->px = calloc(w*h, sizeof(EZ_Px_t));

	return img;
}


void EZ_freeImage(EZ_Image_t* img) {
	free(img->px);
	free(img);
}


EZ_Font_t* EZ_createFont(int w, int h) {
	int i;

	EZ_Font_t* font = malloc( sizeof(EZ_Font_t) );
	font->h = h; 
	font->w = w;
	font->wb = w/9 + 1; /* 1 byte for 8 pixels, at least one byte */

	/* allocate for 256 pointer */
	font->data = malloc( 256 * sizeof(uint8_t*) );

	/* allocate for 256 glyphs */
	font->data[0] = calloc( 256 * font->wb * h, 1);

	/* point each pointer to the desired location */
	for (i = 1; i < 256; i++) 
		font->data[i] = font->data[i-1] + font->wb*h;


	return font;
}



void EZ_freeFont(EZ_Font_t* font) {
	free(font->data[0]);
	free(font->data);
	free(font);
}



EZ_Px_t EZ_randCol() {
	EZ_Px_t col;

	/* col.ref is larger than RAND_MAX ... */
	col.r = (uint8_t)rand();
	col.g = (uint8_t)rand();
	col.b = (uint8_t)rand();
	
	return col;
}


EZ_Px_t EZ_blend(EZ_Px_t fg, EZ_Px_t bg, EZ_BlendMode_t mode) {

	/* https://love2d.org/wiki/BlendMode_Formulas */
	EZ_Px_t result;

	switch (mode) {
		case ALPHA_IGNORE : result = fg; break;
		case ALPHA_FAST   : result = (fg.a == 0) ? bg : fg; break;

		default :
		case ALPHA_BLEND  : 
		
		/* fast integer linear interpolation */
		result.a = fg.a + (255 - fg.a) * bg.a;
		result.r = (fg.a * fg.r + (255 - fg.a) * bg.r) >> 8;
		result.g = (fg.a * fg.g + (255 - fg.a) * bg.g) >> 8;
		result.b = (fg.a * fg.b + (255 - fg.a) * bg.b) >> 8;

		break;
	}

	return result;
}




static void getCanvasRect(__rect__* rec) {

	if (!canvas) return;


	float cvsAspct = canvas->w / canvas->h;
	float winAspct = winWidth / winHeight;

	if (cvsAspct / winAspct > 1) { /* horizontal bars */
		rec->x1 = 0; 
		rec->x2 = winWidth;
		rec->cx = winWidth;

		rec->cy = winWidth / cvsAspct;
		rec->y1 = (winHeight - rec->cy) / 2;
		rec->y2 = winHeight - rec->y1;
	}
	else { /* vertical bars */
		rec->y1 = 0; 
		rec->y2 = winHeight;
		rec->cy = winHeight;

		rec->cx = winHeight * cvsAspct;
		rec->x1 = (winWidth - rec->cx) / 2;
		rec->x2 = winWidth - rec->x1;
	}

}

