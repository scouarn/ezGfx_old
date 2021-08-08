#include "ezGfx.h"
#include "font_editor.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>




void setup(void* param) {

	printf("Loading font %s\n", fname);
	font = EZ_fonts_load(fname);
	editor_font = EZ_fonts_load("res/fonts/norm816.bmpf");

	if (!font.data) {
		printf("Couldn't load font\n");
		exit(EXIT_FAILURE);
	}


	canvas = EZ_createImage(512,512);
	EZ_window("DEMO", 512, 512, canvas);

	x_cols = canvas.w / font.w_px;
	y_cols = canvas.h / font.h_px;

}


void draw(void* param) {

	EZ_draw2D_clear(canvas, EZ_BLUE);

	for (int i = 0; i < _ntabs; i++) EZ_fonts_printStr(canvas, TAB_NAMES[i], editor_font, 
		i == active_tab ? EZ_BLUE : EZ_WHITE, i == active_tab ? EZ_WHITE : EZ_BLUE, 
		i*editor_font.w_px*8, 0, 8, 1);

	DRAW_ROUTINES[active_tab]();
}






void key(void* param) {

	int index = *((int*)param);

	switch (index) {

		case K_F1 ... K_F3 : active_tab = index - K_F1;	break;

		case K_ESCAPE : EZ_stop(); break; //exit

		case K_S : //save
			printf("Saving %s\n", fname);
			EZ_fonts_save(font, fname);
			break;

		case K_O : //open
			break;

		case K_N : //new
			break;

		case K_R : //reload
			break;

		case K_F12 : //screenshot
			printf("Saving screenshot\n");
			EZ_draw2D_saveBMP(canvas, "./screenshot.bmp");
			break;

	}

	KEY_ROUTINES[active_tab](index);


}

void keyRelease(void* param) {}

void mouse(void* param) {}

void kill(void* param) {
  EZ_freeImage(canvas);
  EZ_fonts_free(font);
}



int main (int argc, char **argv) {


	if (argc == 1) {
		printf("No file\n");
		exit(EXIT_FAILURE);
	}
	else {
		strcpy(fname, argv[1]);
	}


	EZ_setCallbak(ON_CREATE, &setup);
	EZ_setCallbak(ON_DRAW,   &draw);
	EZ_setCallbak(ON_CLOSE,  &kill);
	EZ_setCallbak(ON_MOUSEMOVE,   &mouse);
	EZ_setCallbak(ON_KEYPRESSED,  &key);
	EZ_setCallbak(ON_KEYRELEASED, &keyRelease);

	EZ_start();
	EZ_join();


	return 0;
}
