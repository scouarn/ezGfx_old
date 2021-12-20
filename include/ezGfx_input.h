#ifndef _EZGFX_INPUT_H_
#define _EZGFX_INPUT_H_


typedef enum {
	K_ERROR,
	K_LMB, K_RMB, K_MMB,
	K_A, K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N, K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z,
	K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9,
	K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
	K_SPACE, K_TAB, K_BACKSPACE, K_RETURN, K_ESCAPE,
	K_CAPS, K_LSHIFT, K_RSHIFT, K_LCTRL, K_RCTRL, K_LALT, K_RALT,
	K_INS, K_DEL, K_END, K_HOME, K_PGUP, K_PGDN, K_SCROLL, K_PAUSE, K_NUMLOCK,
	K_LEFT, K_UP, K_RIGHT, K_DOWN,
	K_COLON, K_COMMA, K_PERIOD, K_QUOTE, K_SLASH, K_BACKSLASH, K_TILDE, K_OPEN, K_CLOSE, K_PLUS, K_MINUS,
	KP_0, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9, KP_DIV, KP_MUL, KP_PLUS, KP_MINUS, KP_ENTER, KP_DEC,
	_numberOfKeys
} EZ_KeyCode_t;

/*
#define K_MOD_CTRL  0x1
#define K_MOD_SHIFT 0x2
#define K_MOD_ALT   0x4
#define K_MOD_SUPER 0x8
*/

typedef struct {
	bool pressed;	/* has it been pressed on this frame ? */
	bool released;	/* has it been released on this frame ? */
	bool held;		/* is it still held ? */
	char typed;		/* the actual character typed ('\0' if not ascii) */
	/*int  mod;*/		/* bit field with modifier keys */
	EZ_KeyCode_t code;
} EZ_Key_t;

typedef struct {
	int x;		/* position in canvas space */
	int y;
	int dx;		/* change in position since last frame */
	int dy;
	int wheel;	/* +1 or -1 */
} EZ_Mouse_t;


EZ_Key_t*   EZ_getKey(EZ_KeyCode_t code);
EZ_Mouse_t* EZ_getMouse();


#endif /* ezGfx_input_h */