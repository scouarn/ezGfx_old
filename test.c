#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;

void init() {
	printf("init\n");
	canvas = EZ_createImage(256, 256);

	EZ_bind(canvas);
}

void loop(double dt) {
	int i;
	for(i = 0; i < canvas->w * canvas->h; i++)
		canvas->px[i] = EZ_randCol();
}

void kill() {
	EZ_freeImage(canvas);
}


int main() {

	EZ_setCallback_init(init);
	EZ_setCallback_draw(loop);
	EZ_setCallback_kill(kill);

	EZ_start();
	EZ_join();

	printf("SUCCESS.\n");

	return 0;
}