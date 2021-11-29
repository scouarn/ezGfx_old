#include "font_editor.h"


void EZ_callback_init() {

	//load menu font and font to edit
	font = EZ_load_PSF2(fname);
	editor_font = EZ_load_PSF2("custom8x16.psfu");

	//testfont = EZ_load_PSF2("assets/fonts/default8x16.psfu");
	//printf("w %d, h %d, bytes %d\n", testfont.w_px, testfont.h_px, testfont.w_bytes);

	ASSERTM(font.data && editor_font.data, "File error, exit");

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

	//draw menu
	DRAW_ALWAYS();

	//draw active tab
	EZ_draw2D_setTranslate(0, 2*editor_font.h_px);
	DRAW_ROUTINES[active_tab]();
}


void EZ_callback_keyPressed(EZ_Key key) {

	//bindings that are always active
	KEY_ALWAYS(key);

	//bindings only working in the active tab
	KEY_ROUTINES[active_tab](key);


}

void EZ_callback_keyReleased(EZ_Key key) {}

void EZ_callback_mouseMoved(EZ_Mouse mouse) {}


void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_freeFont(font);
  EZ_freeFont(editor_font);
}

EZ_Sample EZ_sfx_callback(double time, int channel) {return (EZ_Sample)0;}



int main (int argc, char **argv) {

	//copy arg
	ASSERTM(argc > 1, "No file");
	strcpy(fname, argv[1]);

	EZ_start();
	EZ_join();


	return 0;
}
