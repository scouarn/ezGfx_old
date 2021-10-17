#include "ezGfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


EZ_Image canvas;
EZ_Font  font;

#define NB_TILES 512
#define BLOCK_SIZE 8
#define NB_PLANES  2 

uint8_t chr_data[NB_TILES*BLOCK_SIZE*NB_PLANES];

char fname[256];


//screen size in chars
uint x_cols, y_cols;



void EZ_callback_init() {

	//load menu font and font to edit
	font = EZ_load_PSF2("font.pfsu");
	
	//read data from input file	
	FILE* fp = fopen(fname, "r");

	if (fp != NULL) {
		fread(chr_data, 1, NB_TILES * BLOCK_SIZE * NB_PLANES, fp);
		fclose(fp);
	}


	//create window
	canvas = EZ_createImage(512,512);
	EZ_bind(canvas);

	//number of glyphs that can fit the screen
	x_cols = canvas.w / font.w_px;
	y_cols = canvas.h / font.h_px;

}


void EZ_callback_draw(double dt) {

	//clear
	EZ_draw2D_clear(canvas, EZ_BLUE);
	EZ_draw2D_setTranslate(0, 0);


	//draw active tab
	EZ_draw2D_setTranslate(0, 2*font.h_px);
}


void EZ_callback_keyPressed(EZ_Key key) {


}

void EZ_callback_keyReleased(EZ_Key key) {}

void EZ_callback_mouseMoved(EZ_Mouse mouse) {}


void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_freeFont(font);
}

EZ_Sample EZ_sfx_callback(double time, int channel) {return (EZ_Sample)0;}



int main (int argc, char **argv) {

	//copy arg
	if(argc > 1)
		strcpy(fname, argv[1]);


	EZ_start();
	EZ_join();


	return 0;
}
