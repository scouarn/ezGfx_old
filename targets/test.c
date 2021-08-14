#include "ezGfx.h"

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

	img = EZ_draw2D_loadBMP("assets/images/test.bmp");

	ASSERTM(img.px, "File error, exit");

	printf("%dx%d\n", img.w, img.h);


	canvas = EZ_createImage(512,512);
	EZ_window(canvas, 512, 512);

	mesh = EZ_draw3D_unitCube();


	EZ_draw2D_clear(canvas, EZ_BLUE);

	proj = PROJ_PERSPECTIVE(HALF_PI, 1.0f, 0.1f, 1000.0f);



}


void EZ_callback_draw(double dt) {
	//printf("%fmillis\n",dt*1000);

	float angle = 0.25 * EZ_getTime();

	transform = TRANS_TRANS(-0.5f, -0.5f, -0.5f);
	transform = MMUL4x4(TRANS_ROTX(angle), 								transform);
	transform = MMUL4x4(TRANS_ROTY(1.33*angle), 					transform);
	transform = MMUL4x4(TRANS_TRANS(0.0f,  0.0f,  10.0f), transform);



	EZ_draw2D_clear(canvas, EZ_BLUE);

	EZ_draw3D_wireframe(canvas, mesh, &proj, &transform);



	// EZ_Poly tri;
	// tri.points[0].pos = (vec3f){-1.0f, -1.0f, -3.0f};
	// tri.points[1].pos = (vec3f){1.0f, 0.0f, -3.0f};
	// tri.points[2].pos = (vec3f){0.5f, 1.0f, -3.0f};

	//EZ_draw3D_tri(canvas, tri, &proj, &transform);




	// EZ_draw2D_line(canvas, EZ_WHITE, 512, 512, 256, 0);
	//EZ_Mouse m = EZ_getMouse();
	//EZ_draw2D_tri(canvas, EZ_RED, (vec2i){m.x, m.y - 10}, (vec2i){m.x + 10, m.y + 10}, (vec2i){m.x - 10, m.y + 10});
	//EZ_draw2D_fillEllipse(canvas, EZ_RED, m.x, m.y, 20, 10);
	//EZ_draw2D_line(canvas, EZ_RED, 300,300, m.x, m.y);
	// EZ_draw2D_fillTri(canvas, EZ_RED, 
	//  	m.x, m.y - 50, 
	//  	m.x + 120, m.y + 60, 
	//  	m.x - 50, m.y + 120
	// );

	//EZ_draw2D_pixel(canvas, c, 100, 100);
	//EZ_draw2D_fillTri(canvas, EZ_RGBA(255,0,0,255), 256, 100, 100, 400, 400, 400);
	//EZ_draw2D_fillRect(canvas, EZ_RED, m.x, m.y, 50, 50);

	//EZ_draw2D_image(canvas, img, m.x, m.y);
	//EZ_draw2D_resizedImage(canvas, img, m.x, m.y, 128, 128);
	//EZ_draw2D_croppedImage(canvas, img, m.x, m.y, 32, 32, 32, 32);

	// AFFMAT trans = AFFAPP(AFFIDT, AFFTNS(100, 100));
	// AFFMAT rot   = AFFAPP(trans,    AFFROT(PI/4));
	

	// //EZ_draw2D_image(canvas, img, 300,300);
	// EZ_draw2D_transformedImage(canvas, img, &rot);



	//EZ_stop();

}



void EZ_callback_keyPressed(EZ_Key key) {


	switch(key.keyCode) {
		case K_ESCAPE : EZ_stop(); break;

		case K_TAB : EZ_draw2D_saveBMP(canvas, "screenshot.bmp");

		case K_LMB :
		{

		}

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
	printf("start\n");
	EZ_start();
	EZ_join();

	return 0;
}
