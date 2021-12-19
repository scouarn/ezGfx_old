#include "ezGfx.h"

#include <math.h>
#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;
EZ_Image_t* texture;
EZ_Mesh_t* cube;
EZ_3DTarget_t* render;
EZ_Mat4_t proj;
EZ_Mat4_t world;
EZ_Mat4_t trans;

float a = 1.0;


void setup() {

	canvas = EZ_image_make(256, 256);

	EZ_mat4_setProj(&proj, QUARTER_PI, (float)WIDTH / HEIGHT, 0.001, 1000);
	EZ_mat4_setId(&world);
	render = EZ_draw3D_makeTarget(canvas, &proj, &world);

	texture = EZ_image_loadBMP("assets/texture.bmp");
	cube = EZ_mesh_loadOBJ("assets/cube.obj");
	// cube = DEBUG_EZ_mesh_unitCube();

	ERR_assert(texture && cube, "Couldn't load assets");

	cube->texture = texture; 


	// EZ_setFullscreen(true);
	EZ_frameRate(60.0);
	EZ_bind(canvas);
}


void draw(double dt) {

	EZ_draw2D_clear(canvas, EZ_BW(51));

	EZ_mat4_setTranslate(&trans, 2.0*cos(a), 2.0*sin(a), 10.0-cos(a));
	EZ_mat4_applyRotX(&trans, a);
	EZ_mat4_applyRotY(&trans, 0.8*a);

	EZ_draw3D_startScene(render);
	EZ_draw3D_mesh(render, cube, &trans, EZ_3D_FLAT_SHADED);

	a += dt;

	EZ_draw2D_image(canvas, texture, 0, 0);
	printf("%lg ms\n", dt*1000.0);
}


void kill() {
	EZ_image_free(canvas);
	EZ_image_free(texture);
	EZ_draw3D_freeTarget(render);
	EZ_mesh_free(cube);
}

void keydown(EZ_Key_t* k) {

	if (k->code == K_F11)
		EZ_setFullscreen(true);

	else if (k->code == K_F12)
		EZ_setFullscreen(false);

	printf("key %d, char %c\n", k->code, k->typed);
}


int main() {

	EZ_setCallback_init(setup);
	EZ_setCallback_draw(draw);
	EZ_setCallback_keyPressed(keydown);

	EZ_start();
	EZ_join();

	return 0;
}