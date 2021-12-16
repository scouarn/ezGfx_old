#include "ezGfx_core_common.h"

#include <pthread.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* App */
static Window win;
static Atom wm_delete;
static pthread_t thread;
static void* mainThread(void* arg);

/* Graphic context */
static Display *disp;
static int screen;
static GC gc;
static char* buffer;


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


void EZ_rename(const char* name) {
	XStoreName(disp, win, name);
}

void EZ_resize(int w, int h) {
	XResizeWindow(disp, win, w, h);
	winWidth = w;
	winHeight = h;
}

void EZ_setFullscreen(bool val) {
	
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


void EZ_redraw() {
	int x, y;
	float xRatio, yRatio;
	__rect__ rec;
	getCanvasRect(&rec);

	

	/* copy canvas to buffer */
	xRatio = (float)canvas->w / rec.cx;
	yRatio = (float)canvas->h / rec.cy;


	/* for each pixel on the screen (that have to be drawn) */
	for (x = rec.x1; x < rec.x2; x++) 
	for (y = rec.y1; y < rec.y2; y++) {

		int sx = (x - rec.x1) * xRatio;
		int sy = (y - rec.y1) * yRatio;

		EZ_Px_t px = canvas->px[sx + sy*canvas->w];

		buffer[(x + y*winWidth)*4   ] = px.b;
		buffer[(x + y*winWidth)*4 +1] = px.g;
		buffer[(x + y*winWidth)*4 +2] = px.r;
	}


	/* draw the buffer on the screen (it has to be rescaled manually) */
	XImage* ximage = XCreateImage(disp, DefaultVisual(disp, screen), 
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



static void* mainThread(void* arg) {
	int i;

	pthread_detach(pthread_self());


	/* create display */
	disp   = XOpenDisplay(NULL);
	ERR_assert(disp, "Couldn't initialize X display");

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
	ERR_assert(XSetWMProtocols(disp, win, &wm_delete, 1), 
			"Couldn't set X11 protocol for handling window destruction");

	/* graphics context */
	gc = XCreateGC(disp, win, 0,0);

	/* init time */
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	double lastTime = EZ_getTime();

	/* init keys */
	for (i = 0; i < _numberOfKeys; i++)
		keyStates[i].code = i;

	/* client init callback */
	if (callback_init) callback_init();
	if (canvas)	EZ_resize(canvas->w, canvas->h);
	else EZ_resize(100, 100);
	
	/* init frame buffer */
	buffer = malloc(winWidth * winHeight * sizeof(EZ_Px_t));


	/* main loop */
	while (running && disp) {

		/* time control */
		double loopBegin = EZ_getTime();


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
				/* XAutoRepeatOff(disp); */
			break;

			case FocusOut:
				/* XAutoRepeatOn(disp); */
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

				if (!canvas) break;

				__rect__ rec;
				getCanvasRect(&rec);

				int winX = e.xmotion.x;
				int winY = e.xmotion.y;

				int newX = (winX - rec.x1) * canvas->w / rec.cx;
				int newY = (winY - rec.y1) * canvas->h / rec.cy;

				if (newX >= 0 && newX < canvas->w
				 && newY >= 0 && newY < canvas->h) {
 					mouseState.x  = newX;
					mouseState.y  = newY;
					mouseState.dx = newX - mouseState.x;
					mouseState.dy = newY - mouseState.y;
					
					if (callback_mouse) callback_mouse( &mouseState );
				}
			}
			break;

			case ConfigureNotify: { /* resize */
				XConfigureEvent req = e.xconfigure;
			
				winWidth = req.width; 
				winHeight = req.height;

				size_t size = winWidth * winHeight * sizeof(EZ_Px_t);

				buffer = realloc(buffer, size);
				memset(buffer, 0, size); /* clear for black bars */

			}
			break;	

			case ClientMessage: /* on destroy */
				if ((Atom)e.xclient.data.l[0] == wm_delete)	EZ_stop();
			break;


			case Expose :

			break;

			}
		}


		/* user function (recalculate delta after waiting) */
		double now = EZ_getTime();
		if (callback_draw) callback_draw(now - lastTime);
		lastTime = now;


		/* display */
		if (canvas) EZ_redraw();


		/* framerate keeping */
		if (frameRate > 0.0) {

			double delta = EZ_getTime() - loopBegin;
			long remain = (1.0 / frameRate - delta) * 1000000;

			if (remain > 0)
				usleep(remain);
		}


	}


	if (callback_kill) callback_kill();
	
	free(buffer);
	XFreeGC(disp, gc);
	XDestroyWindow(disp, win);
	/* XAutoRepeatOn(disp); */
	XCloseDisplay(disp);

	pthread_exit(NULL);

	return NULL;
}
