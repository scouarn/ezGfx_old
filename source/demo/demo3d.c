#include "ezGfx.h"

#include <math.h>
#include <stdio.h>

#define WIDTH  512
#define HEIGHT 512

EZ_Image_t* canvas;
EZ_Mesh_t* cube;
EZ_3DTarget_t* render;
EZ_Mat4_t proj;
EZ_Mat4_t world;
EZ_Mat4_t trans;


float a = 1.0;


void setup() {

	canvas = EZ_image_make(WIDTH, HEIGHT);

	EZ_mat4_setProj(&proj, QUARTER_PI, (float)WIDTH / HEIGHT, 0.001, 1000);
	
	EZ_mat4_setId(&world);
	EZ_mat4_setTranslate(&world, 0, 0, 10.0);

	render = EZ_draw3D_makeTarget(canvas, &proj, &world);

	cube = EZ_mesh_loadOBJ("assets/cube2.obj");


	// EZ_setFullscreen(true);
	EZ_frameRate(60.0);
	EZ_bind(canvas);
}


void draw(double dt) {

	EZ_draw2D_clear(canvas, EZ_BW(51));
	
	EZ_mat4_setId(&trans);
	// EZ_mat4_applyTranslate(&trans, 0, 0, 10.0);
	// EZ_mat4_applyTranslate(&trans, 2.0*cos(a), 2.0*sin(a), 5.0-cos(a));
	EZ_mat4_applyRotX(&trans, a);
	EZ_mat4_applyRotY(&trans, 1.5*a);

	EZ_draw3D_startScene(render);
	EZ_draw3D_mesh(render, cube, &trans);
	EZ_draw3D_endScene(render);


	a += dt;

	// printf("%lg ms\n", dt*1000.0);
}


void kill() {
	EZ_image_free(canvas);
	EZ_draw3D_freeTarget(render);
	EZ_mesh_free(cube);
}

void keydown(EZ_Key_t* k) {

	switch (k->code) {

		case K_F11 : EZ_setFullscreen(true);  break;
		case K_F12 : EZ_setFullscreen(false); break;
		case K_ESCAPE : EZ_stop(); break;
		case K_TAB : EZ_image_saveBMP(canvas, "screenshot.bmp");

		case K_LEFT :
			EZ_mat4_applyTranslate(render->trns, 0.5, 0.0, 0.0);
		break;

		case K_RIGHT :
			EZ_mat4_applyTranslate(render->trns, -0.5, 0.0, 0.0);
		break;

		case K_UP :
			EZ_mat4_applyTranslate(render->trns, 0.0, 0.0, 0.5);
		break;

		case K_DOWN :
			EZ_mat4_applyTranslate(render->trns, 0.0, 0.0, -0.5);
		break;

		case K_Q :
			EZ_mat4_applyRotY(render->trns, -0.1);
		break;

		case K_E :
			EZ_mat4_applyRotY(render->trns, 0.1);
		break;

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