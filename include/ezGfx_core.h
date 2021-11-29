
/*
/
/	EZGFX : MY IDEAL C CROSSPLATFORM GRAPHICS ENGINE HEADER
/ Abstracts platforms and graphics APIs.
/
/ SCOUARN, 2021
/
/	Creates a single window with specified size and attached writable image to draw.
/ Handles keyboard and mouse inputs.
/
/
*/

#ifndef _EZGFX_CORE_H_
#define _EZGFX_CORE_H_

#include <stdbool.h>

#include "ezGfx_input.h"
#include "ezGfx_image.h"
#include "ezGfx_pixel.h"


void EZ_start();	/* start the app thread */
void EZ_stop();		/* force it to stop     */
void EZ_join(); 	/* wait for it to stop  */

void EZ_bind(EZ_Image_t* canvas); /* set what should be drawn */
void EZ_rename(const char*);      /* change the name of the window */
void EZ_resize(int w, int h);     /* change the size of the window */

void EZ_redraw(); /* force screen redraw (will call the draw callback) */
void EZ_setMaximized(bool);  /* maximize window */
void EZ_setFullscreen(bool); /* window fullscreen */
double EZ_getTime(); /* returns time in seconds since start of the thread */



/* CALLBACK FUNCTIONS */
void EZ_setCallback_init( void (*f)(void) );   /* at the start of the thread, before first frame */
void EZ_setCallback_draw( void (*f)(double dt) ); /* called at each frame to update the canvas */
void EZ_setCallback_kill( void (*f)(void) );   /* after the last frame (free memory...) */
void EZ_setCallback_keyPressed(  void (*f)(EZ_Key_t*) );   /* when a key is pressed */
void EZ_setCallback_keyReleased( void (*f)(EZ_Key_t*));	   /* when a key is released */
void EZ_setCallback_mouse(  void (*f)(EZ_Mouse_t*) ); /* when the mouse is moved */

#endif /* ezGfx_core_h */
