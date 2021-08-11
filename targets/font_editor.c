#include "ezGfx.h"
#include "font_editor.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


void EZ_callback_init() {

	font = EZ_draw2D_loadFont(fname);
	editor_font = EZ_draw2D_loadFont("res/fonts/norm816.bmpf");

	ASSERTM(font.data && editor_font.data, "File error, exit");

	canvas = EZ_createImage(512,512);
	EZ_window("DEMO", 512, 512, canvas);

	x_cols = canvas.w / font.w_px;
	y_cols = canvas.h / font.h_px;

}


void EZ_callback_draw(double dt) {

	EZ_draw2D_clear(canvas, EZ_BLUE);
	EZ_draw2D_setTranslate(0, 0);

	DRAW_ALWAYS();

	EZ_draw2D_setTranslate(0, 2*editor_font.h_px);
	DRAW_ROUTINES[active_tab]();
}






void EZ_callback_keyPressed(EZ_Key key) {

	KEY_ALWAYS(key);
	KEY_ROUTINES[active_tab](key);


}

void EZ_callback_keyReleased(EZ_Key key) {}

void EZ_callback_mouseMoved(EZ_Mouse mouse) {}

void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_draw2D_freeFont(font);
}



int main (int argc, char **argv) {

	ASSERTM(argc > 1, "No file");
	strcpy(fname, argv[1]);

	EZ_start();
	EZ_join();


	return 0;
}
