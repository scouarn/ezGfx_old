#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

static EZ_Image img;
static EZ_Image canvas;

void EZ_callback_init() {

	img = EZ_draw2D_loadBMP("res/images/test.bmp");

	ASSERTM(img.px, "File error, exit");

	printf("%dx%d\n", img.w, img.h);


	canvas = EZ_createImage(512,512);
	EZ_window("DEMO", 512, 512, canvas);


}


void EZ_callback_draw(double dt) {

	EZ_draw2D_clear(canvas, EZ_BLUE);

	EZ_Mouse m = EZ_getMouse();
	//EZ_draw2D_tri(canvas, EZ_RED, (vec2i){m.x, m.y - 10}, (vec2i){m.x + 10, m.y + 10}, (vec2i){m.x - 10, m.y + 10});
	//EZ_draw2D_fillEllipse(canvas, EZ_RED, m.x, m.y, 20, 10);
	//EZ_draw2D_line(canvas, EZ_RED, 300,300, m.x, m.y);
	// EZ_draw2D_fillTri(canvas, EZ_RED, 
	//  	m.x, m.y - 50, 
	//  	m.x + 120, m.y + 60, 
	//  	m.x - 50, m.y + 120
	// );

	//EZ_draw2D_pixel(canvas, c, 100, 100);
	EZ_draw2D_fillTri(canvas, EZ_RGBA(255,0,0,255), m.x, m.y, 250, 250, 500, 500);
	//EZ_draw2D_fillRect(canvas, EZ_RED, m.x, m.y, 50, 50);

	//EZ_draw2D_image(canvas, img, m.x, m.y);
	//EZ_draw2D_resizedImage(canvas, img, m.x, m.y, 128, 128);
	//EZ_draw2D_croppedImage(canvas, img, m.x, m.y, 32, 32, 32, 32);

}



void EZ_callback_keyPressed(EZ_Key key) {


	switch(key.keyCode) {
		case K_ESCAPE : EZ_stop(); break;

		case K_TAB : EZ_draw2D_saveBMP(canvas, "screenshot.bmp");


	}

}


void EZ_callback_keyReleased(EZ_Key key) {}

void EZ_callback_mouseMoved(EZ_Mouse mouse) {}

void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_freeImage(img);
}



int main (int argc, char **argv) {

	EZ_start();
	EZ_join();

	return 0;
}
