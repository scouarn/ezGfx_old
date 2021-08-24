#include "ezGfx/ezGfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


static EZ_Image canvas;
static EZ_Font  font;
static EZ_Font  editor_font;


//file name buffer
static char fname[256];

//cursor
static uint32_t edit_char = 'A';
static uint32_t cursor_x, cursor_y, char_cursor_x, char_cursor_y;

//copy paste buffer
static uint32_t buffer[64];

//size in pixels of the "large pixels" in the editor
static const int RES = 16;

//glyph palet size
static const int FW = 32, FH = 8;

//screen size in chars
static int x_cols, y_cols;


//example text
const char* ipsum = "\tLorem ipsum dolor sit amet, consectetur adipiscing elit. Donec quis quam sollicitudin, convallis nisl quis, varius metus. Nullam tempus sit amet odio quis euismod. Suspendisse consequat, neque quis sollicitudin fermentum, erat tortor ultricies erat, volutpat finibus leo tortor non sem. Quisque vehicula mauris suscipit sem porttitor, vel dignissim enim placerat. Aenean maximus vel ipsum eget consequat. Vestibulum suscipit lorem a blandit pellentesque. Nam faucibus tortor nec augue congue rhoncus. Vivamus et metus in eros tincidunt lacinia ac at est. Donec finibus facilisis neque, sed sagittis velit laoreet at. Nullam porta id lectus eget convallis. Pellentesque sagittis erat ipsum.\r\v\t"
					"Vivamus dignissim metus ut molestie mattis. Fusce cursus, turpis ac pulvinar pulvinar, tellus leo volutpat libero, et cursus sem magna quis nisl. Praesent vel blandit elit, non auctor mi. Mauris id lacus ac odio lacinia molestie vitae at urna. Mauris lectus leo, tincidunt vel pretium eget, sollicitudin at dui. Vestibulum eleifend velit justo, eget volutpat lacus convallis sit amet. Aenean orci libero, lobortis at mauris dapibus, consequat rutrum ipsum. Mauris vel sem magna. Integer pretium consectetur velit, ac pharetra metus. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Interdum et malesuada fames ac ante ipsum primis in faucibus.\r\v\t"
					"Sed eu leo accumsan arcu tempus elementum. Sed porta, ante et efficitur varius, nisl felis condimentum leo, quis sodales mauris sapien vel neque. Proin lacinia tellus ut eros euismod, nec venenatis nisi venenatis. Curabitur scelerisque consequat sapien, nec dapibus mi pellentesque malesuada. Mauris purus enim, laoreet sed dictum in, auctor in elit. Sed felis ante, placerat in eros rhoncus, elementum imperdiet risus. Curabitur varius tellus at diam ultrices, id fermentum eros tempor. Etiam molestie ex leo, at tempor libero pharetra eget. Nullam pretium nibh mollis neque faucibus vulputate. Aliquam dictum mi neque, id consectetur tortor pretium semper. Fusce sapien nisi, fermentum ac sodales vel, sodales at diam. Nullam eu elit in metus convallis iaculis. Proin orci mi, faucibus sed sodales tempus, dictum vitae dolor. Suspendisse sed convallis dolor."
;

//help text
const char* help  = "EZ_GFX BITMAP FONT EDITOR v0.1\nScouarn August 2021\n"
					"\vFUNCTION KEYS : Select tab\n"
					"ESC : Exit\nS : Save\nO : Open\nN : New\nR : Reload\nTAB : Screenshot\n"
					"ARROWS : Move cursor\nNUMPAD : Move character select cursor\n"
					"SPACE : Toggle pixel\nRETURN : Edit character\n"
					"C/V : Copy/Paste\nPAGUP/DOWN : Translate up/down\n"	
;

//tabs
static enum {HELP, EDIT, TEST, _ntabs} active_tab = EDIT;


//tab names
const char* const NAME_HELP = "F1 HELP";
const char* const NAME_EDIT = "F2 EDIT";
const char* const NAME_TEST = "F3 TEST";
const char* const TAB_NAMES[] = {NAME_HELP, NAME_EDIT, NAME_TEST};



/* drawing routines for each tab */

static void DRAW_HELP() {
	EZ_draw2D_printStr(canvas, help, editor_font, EZ_WHITE, EZ_BLUE, 0, 0, x_cols, y_cols-2);
}


static void DRAW_EDIT() {

	//font info
	char info[64];
	sprintf(info, "Height : %dpx\nWidth  : %dpx (%db)\nCurrent char : %d x%x \'%c\'", font.h_px, font.w_px, font.w_bytes, edit_char, edit_char, edit_char);
	
	EZ_draw2D_printStr(canvas, info, editor_font, EZ_WHITE, EZ_BLUE, (RES+1)*font.w_px, 0, x_cols/2, 4);

	//display field
	for (int y = 0; y < FH; y++) for (int x = 0; x < FW; x++)    
		EZ_draw2D_printChar(canvas, x+y*FW, font, EZ_WHITE, EZ_BLUE, x*font.w_px, RES*font.h_px + y*font.h_px);   

	//edit char
	for (int y = 0; y < font.h_px; y++)
		for (int xByte = 0; xByte < font.w_bytes; xByte++)
			for (int x = 0; x < 8 && x + 8*xByte < font.w_px; x++)
				if ((font.data[edit_char][xByte + y*font.w_bytes] << x) & 0x80)
					EZ_draw2D_fillRect(canvas, EZ_WHITE, (x + 8*xByte)*RES, y*RES, RES,  RES);

	//cursors
	EZ_draw2D_rect(canvas, EZ_WHITE, 0, 0, RES*font.w_px, RES*font.h_px);
	EZ_draw2D_rect(canvas, EZ_RED, RES*cursor_x, RES*cursor_y, RES,  RES);
	EZ_draw2D_rect(canvas, EZ_GREEN, char_cursor_x*font.w_px, char_cursor_y*font.h_px + RES*font.h_px, font.w_px, font.h_px);
}


static void DRAW_TEST() {
	//example text
	EZ_draw2D_printStr(canvas, ipsum, font, EZ_WHITE, EZ_BLUE, 0, 0, x_cols, y_cols-2);
}


static void DRAW_ALWAYS() {
	//tab names
	for (int i = 0; i < _ntabs; i++) 
		EZ_draw2D_printStr(canvas, TAB_NAMES[i], editor_font, 
			i == active_tab ? EZ_BLUE : EZ_WHITE, i == active_tab ? EZ_WHITE : EZ_BLUE, //COL
			i*editor_font.w_px*8, 0, 8, 1); //POS

}


void (*DRAW_ROUTINES[]) () = {DRAW_HELP, DRAW_EDIT, DRAW_TEST};




/* key handling routines */

static void KEY_HELP(EZ_Key key) {

}

static void KEY_EDIT(EZ_Key key) {

	switch (key.keyCode) {


	//toggle
	#define SET() font.data[edit_char][cursor_y] ^= 0x80 >> cursor_x;

	//if spce held toggle -> allows to paint while moving
	#define SETIF() if (EZ_getKey(K_SPACE).held) SET();

	//move cursor
	case K_LEFT  : cursor_x--; cursor_x %= font.w_px; SETIF(); break;
	case K_UP    : cursor_y--; cursor_y %= font.h_px; SETIF(); break;
	case K_RIGHT : cursor_x++; cursor_x %= font.w_px; SETIF(); break;
	case K_DOWN  : cursor_y++; cursor_y %= font.h_px; SETIF(); break;
		
	//toggle pixel
	case K_SPACE : SET(); break;

	#undef SET
	#undef SETIF
	
	//move character selector
	case KP_4 : char_cursor_x--; char_cursor_x %= FW; break;
	case KP_8 : char_cursor_y--; char_cursor_y %= FH; break;
	case KP_6 :	char_cursor_x++; char_cursor_x %= FW; break;
	case KP_2 : char_cursor_y++; char_cursor_y %= FH; break;

	//select character
	case K_RETURN :	edit_char = char_cursor_x + char_cursor_y*FW; break;

	//move pixels up
	case K_PGUP :
		for (int i = 1; i < font.h_px; i++)
			font.data[edit_char][i-1] = font.data[edit_char][i];

		font.data[edit_char][font.h_px-1] = 0;
	break;

	//move pixels down
	case K_PGDN :
		for (int i = font.h_px-2; i >= 0; i--)
				font.data[edit_char][i+1] = font.data[edit_char][i];

		font.data[edit_char][0] = 0;
	break;


	//copy to buffer
	case K_C :
		for (int y = 0; y < font.h_px; 	  y++)
		for (int x = 0; x < font.w_bytes; x++) {
			int i = x + y * font.w_bytes;
			buffer[i] = font.data[char_cursor_x + char_cursor_y*FW][i];
		}
	break;

	//paste from buffer
	case K_V :
		for (int y = 0; y < font.h_px; 	  y++)
		for (int x = 0; x < font.w_bytes; x++) {
			int i = x + y * font.w_bytes;
			font.data[char_cursor_x + char_cursor_y*FW][i] = buffer[i];
		}
	break;


	default : 
		break;

	}
}

static void KEY_TEST(EZ_Key key) {

}


static void KEY_ALWAYS(EZ_Key key) {

	switch (key.keyCode) {

	//select tab
	case K_F1 ... K_F3 : active_tab = key.keyCode - K_F1; break;

	//exit
	case K_ESCAPE : EZ_stop(); break; //exit

	//save
	case K_S :
		printf("Saving %s\n", fname);
		EZ_save_PSF2(font, fname);

	break;

	//open
	case K_O :
		break;

	//new
	case K_N :
		break;

	//reload
	case K_R : 
		break;

	//screenshot
	case K_TAB : 
		printf("Saving screenshot\n");
		EZ_save_BMP(canvas, "./screenshot.bmp");
	break;

	default : 
		break;
	}
}

void (*KEY_ROUTINES[]) (EZ_Key) = {KEY_HELP, KEY_EDIT, KEY_TEST};