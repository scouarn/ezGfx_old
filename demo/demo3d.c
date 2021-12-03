#include "ezGfx.h"

#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;

void setup() {

	canvas = EZ_createImage(256, 256);
	EZ_bind(canvas);
}

void draw(double dt) {
	EZ_draw2D_clear(canvas, EZ_BW(51));
}

void kill() {
	EZ_freeImage(canvas);
}


int main() {

	EZ_setCallback_init(setup);
	EZ_setCallback_draw(draw);

	EZ_start();
	EZ_join();

	return 0;
}