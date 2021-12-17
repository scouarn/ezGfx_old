#include "ezGfx.h"

#include <math.h>
#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;
EZ_Image_t* texture;


float a = 0.0;


void setup() {

	canvas  = EZ_image_make(256, 256);
	texture = EZ_image_loadBMP("assets/texture.bmp");

	ERR_assert(texture, "Couldn't load assets");

	EZ_frameRate(60.0);
	EZ_bind(canvas);

	printf("w %d  h %d\n", texture->w, texture->h);

}


void draw(double dt) {

	EZ_draw2D_clear(canvas, EZ_BW(51));

	EZ_Mat3_t trans;
	EZ_Mouse_t* m = EZ_getMouse();

	EZ_mat3_setId(&trans);
	EZ_mat3_applyTranslate(&trans, m->x, m->y);
	EZ_mat3_applyScale(&trans, 3, 3);



	EZ_mat3_applyRot(&trans, a);
	EZ_mat3_applyTranslate(&trans, -(texture->w/2.0), -(texture->h/2.0));

	EZ_draw2D_transformedImage(canvas, texture, &trans);

}


void kill() {
	EZ_image_free(canvas);
	EZ_image_free(texture);

}

void keydown(EZ_Key_t* k) {

	switch (k->code) {

		case K_F11 : EZ_setFullscreen(true);  break;
		case K_F12 : EZ_setFullscreen(false); break;

		case K_R : a += 0.1; break;
		case K_T : a -= 0.1; break;

		default : break;
	}
		
}


int main() {

	EZ_setCallback_init(setup);
	EZ_setCallback_draw(draw);
	EZ_setCallback_keyPressed(keydown);

	EZ_start();
	EZ_join();

	return 0;
}