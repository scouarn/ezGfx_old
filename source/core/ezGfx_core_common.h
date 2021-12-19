#ifndef _EZGFX_CORE_COMMON_H_
#define _EZGFX_CORE_COMMON_H_
#else
#error _EZGFX_CORE_COMMON_H_ already defined, this is not a "true header".
#endif

/* Stuff that is not platform dependant but part of the core */

#include "ezGfx_core.h"
#include "ezErr.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Time */
static struct timespec startTime;
static double frameRate = -1.0; /* default negative (unlimited fps) */
static volatile bool running;

/* Drawing */
static EZ_Image_t* canvas;
static bool bFullscreen = false;  
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

void EZ_frameRate(double fps) {
	frameRate = fps;
}

EZ_Key_t* EZ_getKey(EZ_KeyCode_t code) {
	return &keyStates[code];
}

EZ_Mouse_t* EZ_getMouse() {
	return &mouseState;
}



static void getCanvasRect(__rect__* rec) {

	if (!canvas || canvas->h == 0 || winHeight == 0) {
		rec->x1 = 0; rec->y1 = 0;
		rec->x2 = 0; rec->y2 = 0;
		rec->cx = 0; rec->cy = 0;
		
		return;
	}


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

