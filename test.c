#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;

void init() {
	canvas = EZ_createImage(256, 256);

	EZ_bind(canvas);
}


void loop(double dt) {
	int i;
	for(i = 0; i < canvas->w * canvas->h; i++)
		canvas->px[i] = i%2 ? EZ_WHITE : EZ_BLACK;
}

void kill() {
	EZ_freeImage(canvas);
}

void mouse(EZ_Mouse_t* m) {
	printf("x: %d  y: %d\n", m->x, m->y);
}

void key(EZ_Key_t* k) {
	printf("typed %c (%d, %d)\n", k->typed, k->typed, k->keyCode);
}


int main() {

	EZ_setCallback_init(init);
	EZ_setCallback_draw(loop);
	EZ_setCallback_kill(kill);
	EZ_setCallback_mouse(mouse);
	EZ_setCallback_keyPressed(key);

	EZ_start();
	EZ_join();

	printf("SUCCESS.\n");

	return 0;
}