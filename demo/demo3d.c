#include "ezGfx.h"

#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;
EZ_Mesh_t* cube;

mat4x4 proj;

float a = 1.0;

void setup() {

	canvas = EZ_createImage(256, 256);
	cube = EZ_mesh_unitCube();

	proj = PROJ_PERSPECTIVE(QUARTER_PI, (float)WIDTH / HEIGHT, 0.1, 1000);

	EZ_bind(canvas);
}

void draw(double dt) {
	EZ_draw2D_clear(canvas, EZ_BW(51));

	
	mat4x4 translate = TRANS_TRANS(0, 0, 10);
	mat4x4 rotate = mat4x4_MUL(TRANS_ROTX(a), TRANS_ROTY(0.8*a));

	mat4x4 transform = mat4x4_MUL(translate, rotate);

	a += 0.005;

	EZ_draw3D_drawMesh(canvas, cube, &proj, &transform);
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