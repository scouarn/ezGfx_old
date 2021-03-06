
/*
/
/	EZGFX : MY IDEAL C CROSSPLATFORM CONSOLE ENGINE HEADER
/ Abstracts platforms and graphics APIs, depending on the implementation.
/
/ SCOUARN, 2021
/
/	Creates a single window with specified definition and resolution.
/ Handles eyboard inputs.
/
/
*/


#ifndef EZGFX_CORE
#define EZGFX_CORE

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>



/* INPUT FUNCTIONS */
typedef struct {
	bool pressed;
	bool released;
	bool held;
} EZ_Key;

enum EZ_KeyCodes {
	K_ERROR,
	K_LMB, K_RMB, K_MMB,
	K_A, K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N, K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z,
	K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9,
	K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
	K_SPACE, K_TAB, K_BACKSPACE, K_RETURN, K_ESCAPE,
	K_CAPS, K_LSHIFT, K_RSHIFT, K_LCTRL, K_RCTRL, K_LALT, K_RALT,
	K_INS, K_DEL, K_END, K_HOME, K_PGUP, K_PGDN, 	K_SCROLL, K_PAUSE, K_NUMLOCK,
	K_LEFT, K_UP, K_RIGHT, K_DOWN,
	K_COLON, K_COMMA, K_PERIOD, K_QUOTE, K_SLASH, K_BACKSLASH, K_TILDE, K_OPEN, K_CLOSE, K_PLUS, K_MINUS,
	KP_0, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9, KP_DIV, KP_MUL, KP_PLUS, KP_MINUS, KP_ENTER, KP_DEC,
	_numberOfKeys
};
EZ_Key EZ_keyStates[_numberOfKeys];

struct {
	int x;
	int y;
	int dx;
	int dy;
	int wheel;
} EZ_mouse;


/* CALLBACKS */
enum EZ_callbacks {
	ON_CREATE,
	ON_DRAW,
	ON_CLOSE,
	ON_KEYPRESSED,
	ON_KEYRELEASED,
	ON_MOUSEMOVE,

	_numberOfCallbacks //too lazy to change the function pointer declaration each time i add a callback in the enum
};
void(*EZ_callbacks[_numberOfKeys])(void*param); //array of function pointers


/* MAIN FUNCTIONS */
void EZ_start();	//start the loop
void EZ_stop();		//force it to stop
void EZ_join(); 	//wait for it to stop by itself


/* UTILITY */
void EZ_window(const char* name, int w, int h);
void EZ_rename(const char*);
void EZ_resize(int w, int h);

void EZ_setMaximized(bool);
void EZ_setFullscreen(bool);
void EZ_setStretching(bool);


/* TIME FUNCTIONS */
typedef double duration;
duration EZ_getTime();



/* GRAPHIC FUNCITONS */
typedef union {
	uint32_t ref;

	struct {
		uint8_t a;
		uint8_t b;
		uint8_t g;
		uint8_t r;
	} col;
} EZ_px;


typedef struct {
	EZ_px* px; //pixel array

	int w;
	int h;

	void* magic; //left to implementation

} EZ_Image;

EZ_Image* canvas;

EZ_Image* EZ_createImage(int w, int h);
void  	  EZ_freeImage(EZ_Image*);

void  EZ_redraw();
EZ_px EZ_colorRGB(int r, int g, int b);
EZ_px EZ_randCol();
EZ_px EZ_blend(EZ_px colA, EZ_px colB);

#define EZ_BLACK   (EZ_px)0x000000FFU
#define EZ_BLUE    (EZ_px)0x0000FFFFU
#define EZ_GREEN   (EZ_px)0x00FF00FFU
#define EZ_CYAN    (EZ_px)0x00FFFFFFU
#define EZ_RED     (EZ_px)0xFF0000FFU
#define EZ_MAGENTA (EZ_px)0xFF00FFFFU
#define EZ_YELLOW  (EZ_px)0xFFFF00FFU
#define EZ_WHITE   (EZ_px)0xFFFFFFFFU






#endif
