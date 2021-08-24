#include "ezGfx/ezGfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

static EZ_Image img;
static EZ_Image canvas;
static EZ_Mesh  mesh;

static mat4x4 proj;
static mat4x4 transform;


void EZ_callback_init() {

	//load mesh	
	mesh = EZ_load_OBJ("assets/meshes/cube.obj");


	//setup graphics
	canvas = EZ_createImage(800, 600);
	EZ_bind(canvas);

	//init projection matrix
	proj = PROJ_PERSPECTIVE(HALF_PI, (float)canvas.w / canvas.h, 0.1f, 1000.0f);

}


void EZ_callback_draw(double dt) {
	
	
	float angle = 0.5f * EZ_getTime();

	printf("%fmillis\n",dt*1000);

	//init world transform
	transform = ID4x4;
	transform = mat4x4_MUL(TRANS_ROTX(angle), 								transform);
	transform = mat4x4_MUL(TRANS_ROTY(1.33*angle), 						transform);
	//transform = mat4x4_MUL(TRANS_TRANS(cosf(angle),  sinf(angle),   5.0f), transform);
	transform = mat4x4_MUL(TRANS_TRANS(0.0f,  0.0f,   5.0f), transform);
	//transform = mat4x4_MUL(TRANS_ROTY(1.33*angle), 						transform);


	//clear display
	EZ_draw2D_clear(canvas, EZ_BW(51));

	//draw mesh
	EZ_draw3D_drawMesh(canvas, mesh, &proj, &transform);

}



void EZ_callback_keyPressed(EZ_Key key) {


	switch(key.keyCode) {
		
		//exit
		case K_ESCAPE : EZ_stop(); break;

		//screenshot
		case K_TAB : EZ_save_BMP(canvas, "screenshot.bmp");

		//left click
		case K_LMB : break;

		default : 
			break;

	}

}


void EZ_callback_keyReleased(EZ_Key key) {}

void EZ_callback_mouseMoved(EZ_Mouse mouse) {}

void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_freeImage(img);
  // EZ_draw2D_freeFont(font);
  EZ_draw3D_freeMesh(mesh);
}

int main (int argc, char **argv) {

	EZ_start();
	EZ_join();

	return 0;
}
