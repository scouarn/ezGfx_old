#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;

void EZ_callback_init() {

	canvas = EZ_createImage(WIDTH, HEIGHT);

	EZ_bind(canvas);

}

void EZ_callback_draw(double dt) {

	int i;

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		canvas->px[i] = EZ_randCol();		
	}
}


void EZ_callback_kill() {
	EZ_freeImage(canvas);
}


void EZ_callback_keyPressed(EZ_Key_t* key) {}
void EZ_callback_keyReleased(EZ_Key_t* key) {}
void EZ_callback_mouseMoved(EZ_Mouse_t* mouse) {}

int main() {

	EZ_start();
	EZ_join();

	printf("SUCCESS.\n");

	return 0;
}