#include "ezGfx_core.h"
#include "ezGfx_utils.h"

#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


/* Graphic context */
static Display *disp;
static int screen;
static Window win;
static Atom wm_delete;
static GC gc;
static char* buffer;

/* Drawing */
static EZ_Image_t* canvas;
static int winWidth;
static int winHeight;
static float canvasXwinRatio = 1.0f;
static float canvasYwinRatio = 1.0f;
static void _updateBars();

/* Time */
static struct timespec startTime;
static struct timespec lastTime;
static volatile bool running;
static pthread_t thread;
static void* mainThread(void* arg);

/* Inputs */
static EZ_KeyCode_t keyMap(int keyCode);
static EZ_Key_t keyStates[_numberOfKeys];
static EZ_Mouse_t mouseState;

/* Callbacks */
static void (*callback_init)(void);
static void (*callback_draw)(double);
static void (*callback_kill)(void);
static void (*callback_keyPressed)(EZ_Key_t*); 
static void (*callback_keyReleased)(EZ_Key_t*);
static void (*callback_mouseMoved)(EZ_Mouse_t*); 

void EZ_setCallback_init( void (*f)(void) ) {
	callback_init = f;
}

void EZ_setCallback_draw( void (*f)(double dt) ) {
	callback_draw = f;
}

void EZ_setCallback_kill( void (*f)(void) ) {
	callback_kill = f;
}

void EZ_setCallback_keyPressed(  void (*f)(EZ_Key_t*) ) {
	callback_keyPressed = f;
}

void EZ_setCallback_keyReleased( void (*f)(EZ_Key_t*)) {
	callback_keyReleased = f;
}

void EZ_setCallback_mouseMoved(  void (*f)(EZ_Mouse_t*) ) {
	callback_mouseMoved = f;
}


void EZ_bind(EZ_Image_t* cnvs) {
	canvas = cnvs;
}


void EZ_start() {
	running = true;

	/* start engine thread */
	pthread_create(&thread, NULL, &mainThread, NULL);
}

void EZ_stop() {
	/* stop thread */
	running = false;
}

void EZ_join() {
	/* join thread */
	pthread_join(thread, NULL);
}

double EZ_getTime() {
	double time;

	/* accurate time in seconds */
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	time = (now.tv_sec - startTime.tv_sec) + (now.tv_nsec - startTime.tv_nsec) / 1000000000.0;

	return time;
}

EZ_Key_t* EZ_getKey(EZ_KeyCode_t code) {
	return &keyStates[code];
}

EZ_Mouse_t* EZ_getMouse() {
	return &mouseState;
}


void EZ_rename(const char* name) {
	XStoreName(disp, win, name);
}

void EZ_resize(int w, int h) {
	XResizeWindow(disp, win, w, h);
	winWidth = w;
	winHeight = h;
}

void EZ_setFullscreen(bool val) {
	XEvent xev = {0};
	Atom wm_state   = XInternAtom(disp, "_NET_WM_STATE", False);
	Atom fullscreen = XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", False);
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = val; /* true or false */
	xev.xclient.data.l[1] = fullscreen;
	xev.xclient.data.l[2] = 0;
	XSendEvent(disp, DefaultRootWindow(disp), False,
		SubstructureNotifyMask, &xev);
}

void EZ_setMaximized(bool val) {
	XEvent xev = {0};
	Atom wm_state  =  XInternAtom(disp, "_NET_WM_STATE", False);
	Atom max_horz  =  XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
	Atom max_vert  =  XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_VERT", False);
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = val; /* true or false */
	xev.xclient.data.l[1] = max_horz;
	xev.xclient.data.l[2] = max_vert;
	XSendEvent(disp, DefaultRootWindow(disp), False,
		SubstructureNotifyMask, &xev);
}


EZ_Image_t* EZ_createImage(int w, int h) {

	EZ_Image_t* img = malloc( sizeof(EZ_Image_t) );
	
	img->w = w;
	img->h = h;
	img->px = calloc(w*h, sizeof(EZ_Px_t));

	return img;
}


EZ_Image_t* EZ_copyImage(EZ_Image_t* src) {

	size_t nbpx;

	EZ_Image_t* img = malloc( sizeof(EZ_Image_t) );
	
	img->w = src->w;
	img->h = src->h;
	nbpx = img->w * img->h;

	img->px = malloc( nbpx * sizeof(EZ_Px_t) );
	memcpy(img->px, src->px, nbpx * sizeof(EZ_Px_t));

	return img;
}

void EZ_freeImage(EZ_Image_t* img) {
	free(img->px);
	free(img);
}


EZ_Font_t* EZ_createFont(int w, int h) {
	int i;

	EZ_Font_t* font = malloc( sizeof(EZ_Font_t) );
	font->h = h; 
	font->w = w;
	font->wb = w/9 + 1; /* 1 byte for 8 pixels, at least one byte */

	/* allocate for 256 pointer */
	font->data = malloc( 256 * sizeof(uint8_t*) );

	/* allocate for 256 glyphs */
	font->data[0] = calloc( 256 * font->wb * h, 1);

	/* point each pointer to the desired location */
	for (i = 1; i < 256; i++) 
		font->data[i] = font->data[i-1] + font->wb*h;


	return font;
}



void EZ_freeFont(EZ_Font_t* font) {
	free(font->data[0]);
	free(font->data);
	free(font);
}



EZ_Px_t EZ_randCol() {
	EZ_Px_t col;

	/* col.ref is larger than RAND_MAX ... */
	col.r = (uint8_t)rand();
	col.g = (uint8_t)rand();
	col.b = (uint8_t)rand();
	
	return col;
}


EZ_Px_t EZ_blend(EZ_Px_t fg, EZ_Px_t bg, EZ_BlendMode_t mode) {

	/* https://love2d.org/wiki/BlendMode_Formulas */
	EZ_Px_t result;

	switch (mode) {
		case ALPHA_IGNORE : result = fg; break;
		case ALPHA_FAST   : result = (fg.a == 0) ? bg : fg; break;

		default :
		case ALPHA_BLEND  : 
		
		/* fast integer linear interpolation */
		result.a = fg.a + (255 - fg.a) * bg.a;
		result.r = (fg.a * fg.r + (255 - fg.a) * bg.r) >> 8;
		result.g = (fg.a * fg.g + (255 - fg.a) * bg.g) >> 8;
		result.b = (fg.a * fg.b + (255 - fg.a) * bg.b) >> 8;

		break;
	}

	return result;
}


void EZ_redraw() {
	int x1, x2, y1, y2, x, y, sx, sy;
	float xRatio, yRatio;
	EZ_Px_t px;
	XImage* ximage;

	/* copy canvas to buffer */
	x1 = winWidth * (1.0f - canvasXwinRatio) * 0.5f;
	x2 = x1 + winWidth * canvasXwinRatio;

	y1 = winHeight * (1.0f - canvasYwinRatio) * 0.5f;
	y2 = y1 + winHeight * canvasYwinRatio;

	xRatio = (float)canvas->w / (x2-x1);
	yRatio = (float)canvas->h / (y2-y1);


	/* for each pixel on the screen (that have to be drawn) */
	for (x = x1; x < x2; x++) 
	for (y = y1; y < y2; y++) {

		sx = (x-x1) * xRatio;
		sy = (y-y1) * yRatio;

		px = canvas->px[sx + sy*canvas->w];

		buffer[(x + y*winWidth)*4   ] = px.b;
		buffer[(x + y*winWidth)*4 +1] = px.g;
		buffer[(x + y*winWidth)*4 +2] = px.r;

	}


	/* draw the buffer on the screen (it has to be rescaled manually) */
	ximage = XCreateImage(disp, DefaultVisual(disp, screen), 
		24, ZPixmap, 0, buffer, winWidth, winHeight, 32, 0);

	XPutImage(disp, win, gc, ximage, 0, 0, 0, 0, winWidth, winHeight);

}






/* PRIVATE */


static EZ_KeyCode_t keyMap(int keyCode) {
	/* https://cgit.freedesktop.org/xorg/proto/x11proto/tree/keysymdef.h */

	switch (keyCode) {
		case 0x41 : return K_SPACE;   case 0x16 : return K_BACKSPACE;
		case 0x09 : return K_ESCAPE;  case 0x17 : return K_TAB;
		case 0x24 : return K_RETURN;
		case 0x32 : return K_LSHIFT;  case 0x3e : return K_RSHIFT;
		case 0x25 : return K_LCTRL;   case 0x69 : return K_RCTRL;
		case 0x40 : return K_LALT;    case 0x6c : return K_RALT;
		case 0x42 : return K_CAPS;    case 0x4d : return K_NUMLOCK;
		case 0x4e : return K_SCROLL;  case 0x7f : return K_PAUSE;
		case 0x76 : return K_INS;     case 0x77 : return K_DEL;
		case 0x6e : return K_HOME;    case 0x73 : return K_END;
		case 0x70 : return K_PGUP;    case 0x75 : return K_PGDN;
		case 0x71 : return K_LEFT;    case 0x6f : return K_UP;
		case 0x72 : return K_RIGHT;   case 0x74 : return K_DOWN;

		case 0x3c : return K_PERIOD;
		case 0x2f : return K_COLON;   case 0x3b : return K_COMMA;
		case 0x30 : return K_QUOTE;   case 0x31 : return K_TILDE;
		case 0x3d : return K_SLASH;   case 0x33 :  return K_BACKSLASH;
		case 0x22 : return K_OPEN;    case 0x23 : return K_CLOSE;
		case 0x15 : return K_PLUS;    case 0x14 : return K_MINUS;

		case 0x5b : return KP_DEC;	case 0x68 : return K_RETURN;
		case 0x3f : return KP_MUL;  case 0x6a : return KP_DIV;
		case 0x56 : return K_PLUS;  case 0x52 : return K_MINUS;

		case 0x5a : return KP_0;  case 0x57 : return KP_1;  case 0x58 : return KP_2;
		case 0x59 : return KP_3;  case 0x53 : return KP_4;  case 0x54 : return KP_5;
		case 0x55 : return KP_6;  case 0x4f : return KP_7;  case 0x50 : return KP_8;
		case 0x51 : return KP_9;

		case 0x13 : return K_0;   case 0x0a : return K_1;   case 0x0b : return K_2;
		case 0x0c : return K_3;   case 0x0d : return K_4;   case 0x0e : return K_5;
		case 0x0f : return K_6;   case 0x10 : return K_7;   case 0x11 : return K_8;
		case 0x12 : return K_9;

		case 0x43 : return K_F1;  case 0x44 : return K_F2;  case 0x45 : return K_F3;
		case 0x46 : return K_F4;  case 0x47 : return K_F5;  case 0x48 : return K_F6;
		case 0x49 : return K_F7;  case 0x4a : return K_F8;  case 0x4b : return K_F9;
		case 0x4c : return K_F10; case 0x5f : return K_F11; case 0x60 : return K_F12;

		case 0x18 : return K_Q;   case 0x19 : return K_W;   case 0x1a : return K_E;
		case 0x1b : return K_R;   case 0x1c : return K_T;   case 0x1d : return K_Y;
		case 0x1e : return K_U;   case 0x1f : return K_I;   case 0x20 : return K_O;
		case 0x21 : return K_P;   case 0x26 : return K_A;   case 0x27 : return K_S;
		case 0x28 : return K_D;   case 0x29 : return K_F;   case 0x2a : return K_G;
		case 0x2b : return K_H;   case 0x2c : return K_J;   case 0x2d : return K_K;
		case 0x2e : return K_L;   case 0x34 : return K_Z;   case 0x35 : return K_X;
		case 0x36 : return K_C;   case 0x37 : return K_V;   case 0x38 : return K_B;
		case 0x39 : return K_N;   case 0x3a : return K_M;

		default : return K_ERROR;
	}
}



static void _updateBars() {
	float canvasRatio, windowRatio;

	canvasRatio = (float)canvas->w / canvas->h;
	windowRatio = (float)winWidth / winHeight;
	canvasXwinRatio = canvasRatio / windowRatio;
	canvasYwinRatio = windowRatio / canvasRatio;

	if (canvasXwinRatio > 1.0f)
		canvasXwinRatio = 1.0f;

	else
		canvasYwinRatio = 1.0f;

}


static void* mainThread(void* arg) {
	int i;

	pthread_detach(pthread_self());


	/* create display */
	disp   = XOpenDisplay(NULL);
	EZ_assert(disp, "Couldn't initialize X display");

	/* get screen */
	screen = DefaultScreen(disp);

	/* create window */
	win = XCreateSimpleWindow(disp, RootWindow(disp, screen), 
		0, 0, 100, 100, 1, BlackPixel(disp, screen), 
		BlackPixel(disp, screen));
	

	/* configure window */
	XStoreName(disp, win, "EZGFX");	
	XSelectInput(disp, win, StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | FocusChangeMask);
	XMapWindow(disp, win);

	/* window destruction handling */
	wm_delete = XInternAtom(disp, "WM_DELETE_WINDOW", True);
	EZ_assert(XSetWMProtocols(disp, win, &wm_delete, 1), 
			"Couldn't set X11 protocol for handling window destruction");

	/* graphics context */
	gc = XCreateGC(disp, win, 0,0);

	/* init frame buffer */
	buffer = calloc(winWidth * winHeight, 4);


	/* init time */
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	clock_gettime(CLOCK_MONOTONIC, &lastTime);

	/* init keys */
	for (i = 0; i < _numberOfKeys; i++)
		keyStates[i].keyCode = i;

	/* client init callback */
	if (callback_init) callback_init();
	if (canvas) {
		EZ_resize(canvas->w, canvas->h);
		_updateBars();
	}
	else EZ_resize(100, 100);
	



	/* main loop */
	while (running && disp) {

		
		/* elapsed time */
		struct timespec now;
		double elapsedTime;
		clock_gettime(CLOCK_MONOTONIC, &now);
		elapsedTime = (now.tv_sec - lastTime.tv_sec) + (now.tv_nsec - lastTime.tv_nsec) / 1000000000.0;
		lastTime = now;

		/* user function */
		if (callback_draw) callback_draw(elapsedTime);


		/* update keystates */
		for (i = 0; i < _numberOfKeys; i++) {
			keyStates[i].pressed  = false;
			keyStates[i].released = false;
		}
		mouseState.wheel = 0;
		/* events and inputs */
		while (XPending(disp)) {
			XEvent e;
			XNextEvent(disp, &e);

			switch (e.type) {


			case KeyPress: {

				EZ_KeyCode_t index = keyMap(e.xkey.keycode);
				EZ_Key_t* key = &(keyStates[index]);

				key->pressed = true;
				key->held    = true;

				XLookupString(&e.xkey, &(key->typed), 1, NULL, NULL);

				if (callback_keyPressed) callback_keyPressed(key);

				break;
			}


			case KeyRelease: {
			
				EZ_KeyCode_t index = keyMap(e.xkey.keycode);
				EZ_Key_t* key = &(keyStates[index]);

				key->released = true;
				key->held = false;
				XLookupString(&e.xkey, &(key->typed), 1, NULL, NULL);

				if (callback_keyReleased) callback_keyReleased(key);
				break;
			}

			case FocusIn :
				/* XAutoRepeatOff(disp); /* turn off autistic keyboard inputs */
			break;

			case FocusOut:
				/* XAutoRepeatOn(disp); /* put it back on so other apps are not affected */
			break;

			case ButtonPress: {
				int sym = e.xbutton.button;
				EZ_KeyCode_t index = 0;
				switch (sym) {
					case 1:	keyStates[K_LMB].pressed = true; keyStates[K_LMB].held = true; index = K_LMB; break;
					case 2:	keyStates[K_MMB].pressed = true; keyStates[K_MMB].held = true; index = K_MMB; break;
					case 3:	keyStates[K_RMB].pressed = true; keyStates[K_RMB].held = true; index = K_RMB; break;
					case 4:	mouseState.wheel = 1; break;
					case 5:	mouseState.wheel =-1; break;
					default: break;
				}
				
				if (callback_keyPressed) callback_keyPressed( &(keyStates[index]) );
				break;
			}

			case ButtonRelease: {
				int sym = e.xbutton.button;
				EZ_KeyCode_t index = 0;
				
				switch (sym) {
					case 1:	keyStates[K_LMB].released = true; keyStates[K_LMB].held = false; index = K_LMB; break;
					case 2:	keyStates[K_MMB].released = true; keyStates[K_MMB].held = false; index = K_MMB; break;
					case 3:	keyStates[K_RMB].released = true; keyStates[K_RMB].held = false; index = K_RMB; break;
					default: break;	
				}

				if (callback_keyReleased) callback_keyReleased( &(keyStates[index]) );
				break;
			}

			case MotionNotify: {

				if (canvas) {
					int newx = (e.xmotion.x + 0.5f*(canvasXwinRatio - 1.0f) * winWidth ) * canvas->w / ((float)winWidth  * canvasXwinRatio);
					int newy = (e.xmotion.y + 0.5f*(canvasYwinRatio - 1.0f) * winHeight) * canvas->h / ((float)winHeight * canvasYwinRatio);

					mouseState.dx = newx - mouseState.x;
					mouseState.dy = newy - mouseState.y;
					mouseState.x  = newx;
					mouseState.y  = newy;
				}

				if (callback_mouseMoved) callback_mouseMoved( &mouseState );
				break;
			}

			case ConfigureNotify: { /* resize */

				XConfigureEvent req = e.xconfigure;
				winWidth = req.width; winHeight = req.height;
				free(buffer);
				buffer = calloc(winWidth*winHeight, 4);
				
				if (canvas) _updateBars();

				break;
			}

			case ClientMessage: { /* on destroy */

				if ((Atom)e.xclient.data.l[0] == wm_delete)	EZ_stop();
				break;
			}

			}
		}

		/* display */
		if (canvas) EZ_redraw();


	}


	if (callback_kill) callback_kill();
	
	free(buffer);
	XFreeGC(disp, gc);
	XDestroyWindow(disp, win);
	/* XAutoRepeatOn(disp); /* put it back on so other apps are not affected */
	XCloseDisplay(disp);

	pthread_exit(NULL);

	return NULL;
}
