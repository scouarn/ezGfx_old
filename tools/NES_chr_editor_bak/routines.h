
const EZ_Px NES_palette[256] = {

	EZ_BLACK,
	EZ_RED,
	EZ_GREEN,
	EZ_YELLOW,
	EZ_WHITE

};



//cursor
unsigned int edit_char = 0;
unsigned int cursor_x, cursor_y, char_cursor_x, char_cursor_y;

unsigned char palette[4] = {0, 1, 2, 3};


//copy paste buffer
unsigned char buffer[BLOCK_SIZE*2];

//size in pixels of the "large pixels" in the editor
#define RES 16

//glyph palet size
#define FW 16
#define FH 16


//screen size in chars
unsigned int x_cols, y_cols;


//example text
const char* ipsum = "\tLorem ipsum dolor sit amet, consectetur adipiscing elit. Donec quis quam sollicitudin, convallis nisl quis, varius metus. Nullam tempus sit amet odio quis euismod. Suspendisse consequat, neque quis sollicitudin fermentum, erat tortor ultricies erat, volutpat finibus leo tortor non sem. Quisque vehicula mauris suscipit sem porttitor, vel dignissim enim placerat. Aenean maximus vel ipsum eget consequat. Vestibulum suscipit lorem a blandit pellentesque. Nam faucibus tortor nec augue congue rhoncus. Vivamus et metus in eros tincidunt lacinia ac at est. Donec finibus facilisis neque, sed sagittis velit laoreet at. Nullam porta id lectus eget convallis. Pellentesque sagittis erat ipsum.\r\v\t"
					"Vivamus dignissim metus ut molestie mattis. Fusce cursus, turpis ac pulvinar pulvinar, tellus leo volutpat libero, et cursus sem magna quis nisl. Praesent vel blandit elit, non auctor mi. Mauris id lacus ac odio lacinia molestie vitae at urna. Mauris lectus leo, tincidunt vel pretium eget, sollicitudin at dui. Vestibulum eleifend velit justo, eget volutpat lacus convallis sit amet. Aenean orci libero, lobortis at mauris dapibus, consequat rutrum ipsum. Mauris vel sem magna. Integer pretium consectetur velit, ac pharetra metus. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Interdum et malesuada fames ac ante ipsum primis in faucibus.\r\v\t"
					"Sed eu leo accumsan arcu tempus elementum. Sed porta, ante et efficitur varius, nisl felis condimentum leo, quis sodales mauris sapien vel neque. Proin lacinia tellus ut eros euismod, nec venenatis nisi venenatis. Curabitur scelerisque consequat sapien, nec dapibus mi pellentesque malesuada. Mauris purus enim, laoreet sed dictum in, auctor in elit. Sed felis ante, placerat in eros rhoncus, elementum imperdiet risus. Curabitur varius tellus at diam ultrices, id fermentum eros tempor. Etiam molestie ex leo, at tempor libero pharetra eget. Nullam pretium nibh mollis neque faucibus vulputate. Aliquam dictum mi neque, id consectetur tortor pretium semper. Fusce sapien nisi, fermentum ac sodales vel, sodales at diam. Nullam eu elit in metus convallis iaculis. Proin orci mi, faucibus sed sodales tempus, dictum vitae dolor. Suspendisse sed convallis dolor."
;

//help text
const char* help  = "EZ_GFX NES CHARACTER EDITOR v0.1\nScouarn October 2021\n"
					"\vFUNCTION KEYS : Select tab\n"
					"ESC : Exit\nS : Save\nTAB : Screenshot\n"
					"ARROWS : Move cursor\nNUMPAD : Select\n"
					"Num keys : paint\nRETURN : Edit\n"
					"C/V : Copy/Paste\nPAGUP/DOWN : Cycle pages\n"
;

//tabs
static enum {HELP, EDIT, TEST, _ntabs} active_tab = EDIT;


//tab names
const char* const NAME_HELP = "F1 HELP";
const char* const NAME_EDIT = "F2 EDIT";
const char* const NAME_TEST = "F3 TEST";
const char* TAB_NAMES[] = {NAME_HELP, NAME_EDIT, NAME_TEST};





static void draw_block(unsigned int id, int x0, int y0, int res) {


	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * 2);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	for (int y = 0; y < BLOCK_SIZE; y++)
	for (int x = 0; x < BLOCK_SIZE; x++) {

		//take to two bits corresponding to the color information
		unsigned char col_id = 0;
		col_id |= (  plane1[y] >> (BLOCK_SIZE - x - 1)) & 0x1;
		col_id |= (( plane2[y] >> (BLOCK_SIZE - x - 1)) & 0x1) << 1;

		EZ_Px col = NES_palette[palette[col_id]];

		EZ_draw2D_fillRect(canvas, col, (x0+x) * res, (y0+y) * res, res, res);

	}

}




/* drawing routines for each tab */

static void DRAW_HELP() {
	EZ_draw2D_printStr(canvas, help, font, EZ_WHITE, EZ_BLUE, 0, 0, x_cols, y_cols-2);
}


static void DRAW_EDIT() {

	//font info
	char info[64];
	sprintf(info, "X %02d  Y %02d (Id %02d)\nEditing %02d\nPalette %02X %02X %02X %02X\n", 
		char_cursor_x, char_cursor_y, 
		char_cursor_x + char_cursor_y * FW, 
		edit_char,
		palette[0], palette[1], palette[2], palette[3]
	);

	EZ_draw2D_printStr(canvas, info, font, EZ_WHITE, EZ_BLUE, (RES+1)*font.w_px, 0, x_cols/2, 4);


	//palette
	for (int i = 0; i < 4; i++) {
		EZ_Px col = NES_palette[palette[i]];
		EZ_draw2D_fillRect(canvas, col, (1+BLOCK_SIZE)*RES + i*RES*2, (BLOCK_SIZE/2)*RES, 2*RES, 2*RES);
	}



	//display page
	for (int y = 0; y < FH; y++)
	for (int x = 0; x < FW; x++) {
		draw_block(x+y*FW, x*BLOCK_SIZE, RES*BLOCK_SIZE + y*BLOCK_SIZE, 1);
	}


	//edit char
	draw_block(edit_char, 0, 0, RES);


	//white border
	EZ_draw2D_rect(canvas, EZ_WHITE, 0, 0, RES*BLOCK_SIZE, RES*BLOCK_SIZE);

	//edit cursor
	EZ_draw2D_rect(canvas, EZ_RED, RES*cursor_x, RES*cursor_y, RES,  RES);

	//select cursor
	EZ_draw2D_rect(canvas, EZ_GREEN, char_cursor_x*BLOCK_SIZE, char_cursor_y*BLOCK_SIZE + RES*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
}


static void DRAW_TEST() {
	//example text
	EZ_draw2D_printStr(canvas, ipsum, font, EZ_WHITE, EZ_BLUE, 0, 0, x_cols, y_cols-2);
}


static void DRAW_ALWAYS() {
	//tab names
	for (int i = 0; i < _ntabs; i++) 
		EZ_draw2D_printStr(canvas, TAB_NAMES[i], font, 
			i == active_tab ? EZ_BLUE : EZ_WHITE, i == active_tab ? EZ_WHITE : EZ_BLUE, //COL
			i*font.w_px*8, 0, 8, 1); //POS

}


void (*DRAW_ROUTINES[]) () = {DRAW_HELP, DRAW_EDIT, DRAW_TEST};




/* key handling routines */

static void KEY_HELP(EZ_Key key) {

}

static void KEY_EDIT(EZ_Key key) {

	switch (key.keyCode) {



	//move cursor
	case K_LEFT  : cursor_x--; cursor_x %= BLOCK_SIZE; break;
	case K_UP    : cursor_y--; cursor_y %= BLOCK_SIZE; break;
	case K_RIGHT : cursor_x++; cursor_x %= BLOCK_SIZE; break;
	case K_DOWN  : cursor_y++; cursor_y %= BLOCK_SIZE; break;
		

	//move character selector
	case KP_4 : char_cursor_x--; char_cursor_x %= FW; break;
	case KP_8 : char_cursor_y--; char_cursor_y %= FH; break;
	case KP_6 :	char_cursor_x++; char_cursor_x %= FW; break;
	case KP_2 : char_cursor_y++; char_cursor_y %= FH; break;

	//select character
	case K_RETURN :	
		edit_char = char_cursor_x + char_cursor_y*FW; 

	break;

	//
	case K_PGUP :
		
	break;

	//
	case K_PGDN :

	break;


	//copy to buffer
	case K_C :

	break;

	//paste from buffer
	case K_V :

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
		
		FILE* fp = fopen(fname, "w");

		if (fp != NULL) {
			fwrite(chr_data, 1, NB_TILES * BLOCK_SIZE * 2, fp);
			fclose(fp);
		}
		else 
			printf("Couldn't write to file\n");


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