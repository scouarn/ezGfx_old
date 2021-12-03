#include "ezGfx.h"

#include <stdio.h>

#define WIDTH 256
#define HEIGHT 256

EZ_Image_t* canvas;
EZ_Mesh_t* cube;

EZ_Mat4_t proj;
EZ_Mat4_t trans;

float a = 1.0;

void setup() {

	canvas = EZ_createImage(256, 256);
	cube = EZ_mesh_unitCube();

	EZ_mat4_setProj(&proj, QUARTER_PI, (float)WIDTH / HEIGHT, 0.1, 1000);

	EZ_bind(canvas);
}

void draw(double dt) {
	EZ_draw2D_clear(canvas, EZ_BW(51));

	EZ_mat4_setTranslate(&trans, 0.0, 0.0, 10.0);
	EZ_mat4_applyRotX(&trans, a);
	EZ_mat4_applyRotY(&trans, 0.8*a);

	a += 0.005;

	EZ_draw3D_drawMesh(canvas, cube, &proj, &trans);

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