#include "ezGfx_core_common.h"

#include <pthread.h>
#include <unistd.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>	
#include <errno.h>


#define DEV_FRAME_BUFFER "/dev/fb0"
#define DEV_TERM         "/dev/console"
#define DEV_MOUSE        "/dev/input/event7"
#define DEV_KEYBOARD     "/dev/input/by-id/usb-SEM_USB_Keyboard-event-kbd"



/* App */
static pthread_t thread;
static void* mainThread(void* arg);
static int hTerm;
static int hMouse;
static int hKb;

/* Graphic context */
static char* fbmem;
static char* buffer;
static int hFb;
static struct fb_var_screeninfo screenInfo;



void EZ_bind(EZ_Image_t* cnvs) {
	canvas = cnvs;
}

void EZ_start() {
	running = true;

	/* start engine thread */
	pthread_create(&thread, NULL, mainThread, NULL);
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

}

void EZ_resize(int w, int h) {

}

void EZ_setFullscreen(bool val) {
	bFullscreen = val;
}

void EZ_setMaximized(bool val) {

}


void EZ_redraw() {
	int x, y;
	float xRatio, yRatio;
	__rect__ rec;
	getCanvasRect(&rec);

	/* copy canvas to frame buffer */
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

	/* writing directly to fbmem instead of copying is SLOWER
	   and cannot achevent.e more than 30 fps on my machine */
	memcpy(fbmem, buffer, winWidth * winHeight * 4);
}







static EZ_KeyCode_t keyMap(int keyCode) {

	switch (keyCode) {
		
		case KEY_SPACE :      return K_SPACE;   case KEY_BACKSPACE : return K_BACKSPACE;
		case KEY_ESC :        return K_ESCAPE;  case KEY_TAB :       return K_TAB;
		case KEY_ENTER :      return K_RETURN;

		case KEY_LEFTSHIFT :  return K_LSHIFT;  case KEY_RIGHTSHIFT : return K_RSHIFT;
		case KEY_LEFTCTRL :   return K_LCTRL;   case KEY_RIGHTCTRL :  return K_RCTRL;
		case KEY_LEFTALT :    return K_LALT;    case KEY_RIGHTALT :   return K_RALT;

		case KEY_CAPSLOCK :   return K_CAPS;    case KEY_NUMLOCK : return K_NUMLOCK;
		case KEY_SCROLLLOCK : return K_SCROLL;  case KEY_PAUSE :   return K_PAUSE;
		case KEY_INSERT :     return K_INS;     case KEY_DELETE :  return K_DEL;
		
		case KEY_HOME :       return K_HOME;    case KEY_END : return K_END;
		case KEY_PAGEUP :     return K_PGUP;    case KEY_PAGEDOWN : return K_PGDN;
		
		case KEY_LEFT :       return K_LEFT;    case KEY_UP : return K_UP;
		case KEY_RIGHT :      return K_RIGHT;   case KEY_DOWN : return K_DOWN;

		case KEY_DOT :        return K_PERIOD;
		case KEY_SEMICOLON :  return K_COLON;   case KEY_COMMA :      return K_COMMA;
		case KEY_APOSTROPHE : return K_QUOTE;   case KEY_GRAVE :      return K_TILDE;
		case KEY_SLASH :      return K_SLASH;   case KEY_BACKSLASH :  return K_BACKSLASH;
		case KEY_LEFTBRACE :  return K_OPEN;    case KEY_RIGHTBRACE : return K_CLOSE;
		case KEY_EQUAL :      return K_PLUS;    case KEY_MINUS :      return K_MINUS;

		case KEY_KPDOT :      return KP_DEC;  case KEY_KPENTER : return KP_ENTER;
		case KEY_KPASTERISK : return KP_MUL;  case KEY_KPSLASH : return KP_DIV;
		case KEY_KPPLUS :     return KP_PLUS; case KEY_KPMINUS : return K_MINUS;

		case KEY_KP0 : return KP_0;  case KEY_KP1 : return KP_1;  case KEY_KP2 : return KP_2;
		case KEY_KP3 : return KP_3;  case KEY_KP4 : return KP_4;  case KEY_KP5 : return KP_5;
		case KEY_KP6 : return KP_6;  case KEY_KP7 : return KP_7;  case KEY_KP8 : return KP_8;
		case KEY_KP9 : return KP_9;

		case KEY_0 : return K_0;   case KEY_1 : return K_1;   case KEY_2 : return K_2;
		case KEY_3 : return K_3;   case KEY_4 : return K_4;   case KEY_5 : return K_5;
		case KEY_6 : return K_6;   case KEY_7 : return K_7;   case KEY_8 : return K_8;
		case KEY_9 : return K_9;

		case KEY_F1  : return K_F1;  case KEY_F2  : return K_F2;  case KEY_F3  : return K_F3;
		case KEY_F4  : return K_F4;  case KEY_F5  : return K_F5;  case KEY_F6  : return K_F6;
		case KEY_F7  : return K_F7;  case KEY_F8  : return K_F8;  case KEY_F9  : return K_F9;
		case KEY_F10 : return K_F10; case KEY_F11 : return K_F11; case KEY_F12 : return K_F12;

		case KEY_Q : return K_Q;   case KEY_W : return K_W;   case KEY_E : return K_E;
		case KEY_R : return K_R;   case KEY_T : return K_T;   case KEY_Y : return K_Y;
		case KEY_U : return K_U;   case KEY_I : return K_I;   case KEY_O : return K_O;
		case KEY_P : return K_P;   case KEY_A : return K_A;   case KEY_S : return K_S;
		case KEY_D : return K_D;   case KEY_F : return K_F;   case KEY_G : return K_G;
		case KEY_H : return K_H;   case KEY_J : return K_J;   case KEY_K : return K_K;
		case KEY_L : return K_L;   case KEY_Z : return K_Z;   case KEY_X : return K_X;
		case KEY_C : return K_C;   case KEY_V : return K_V;   case KEY_B : return K_B;
		case KEY_N : return K_N;   case KEY_M : return K_M;

		default : return K_ERROR;

	}
}


void __failsafe__(int sig) {
	EZ_stop();
}

static void* mainThread(void* arg) {
	int i;


	/* https://docs.huihoo.com/doxygen/linux/kernel/3.7/structfb__var__screeninfo.html */
	/* https://cmcenroe.me/2018/01/30/fbclock.html */
	/* https://jiafei427.wordpress.com/2017/03/13/linux-reading-the-mouse-events-datas-from-devinputmouse0/ */


	/* open frame buffer device */
	ERR_assert(-1 != (hFb = open(DEV_FRAME_BUFFER, O_RDWR)), 
		"Couldn't open frame buffer, maybe try adding yourself to the video group");

	/* get screen info */
	ERR_assert(-1 != ioctl(hFb, FBIOGET_VSCREENINFO, &screenInfo),
		"Couldn't get screen info");

	ERR_assert(screenInfo.bits_per_pixel == 32, 
		"Unsupported bits pixel format");

	winWidth  = screenInfo.xres; 
	winHeight = screenInfo.yres;

	/* get handle to screen buffer */
	size_t buffSize = 4 * screenInfo.xres * screenInfo.yres;
	fbmem = mmap(NULL, buffSize, PROT_READ | PROT_WRITE, MAP_SHARED, hFb, 0);

	ERR_assert(fbmem != MAP_FAILED, 
		"Couldn't map the frame buffer");


	/* init buffers */
	memset(fbmem, 0x00, buffSize);
	buffer = calloc(buffSize, 1);



	/* Open mouse device */
	ERR_assert(-1 != (hMouse = open(DEV_MOUSE, O_RDONLY | O_NONBLOCK)), 
		"Couldn't open mouse device.");

	mouseState.x = 0;
	mouseState.y = 0;


	/* Open keyboard */
	ERR_assert(-1 != (hKb = open(DEV_KEYBOARD, O_RDONLY | O_NONBLOCK)), 
		"Couldn't open keyboard device.");

	for (i = 0; i < _numberOfKeys; i++)
		keyStates[i].code = i;


	/* open the console device */
	ERR_assert(-1 != (hTerm = open(DEV_TERM, O_NOCTTY)),
		"Failed to open console device.");

	/* switch to graphics mode */
	ERR_assert(-1 != ioctl(hTerm, KDSETMODE, KD_GRAPHICS),
		"Failed to enter graphics mode");


	/* override interrupts so it goes back to text mode CRITICAL! */
	signal(SIGABRT, __failsafe__); 
	signal(SIGFPE,  __failsafe__);
	signal(SIGILL,  __failsafe__);
	signal(SIGINT,  __failsafe__);
	signal(SIGTERM, __failsafe__);



	/* init time */
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	double lastTime = EZ_getTime();

	/* clevent.t init callback */
	if (callback_init) callback_init();



	/* main loop */
	while (running) {

		/* time control */
		double loopBegin = EZ_getTime();

		struct input_event ev;

		/* Handle mouse events */
		mouseState.wheel = 0;
		mouseState.dx = 0;
		mouseState.dy = 0;

		while(0 < read(hMouse, &ev, sizeof(struct input_event))  ) {

			if (ev.type == EV_REL)
			switch (ev.code) {

				case REL_X :
					mouseState.dx = ev.value;
				break;

				case REL_Y :
					mouseState.dy = ev.value;
				break;

				case REL_WHEEL :
					mouseState.wheel = ev.value;
				break;

			}

		}

		ERR_assert(errno == EAGAIN, "Error while reading mouse device.");

		if (mouseState.dx != 0
		 || mouseState.dy != 0
		 || mouseState.wheel != 0) {
			
			int maxW = canvas ? canvas->w : 100;
			int maxH = canvas ? canvas->h : 100;

			mouseState.x = CLAMP(mouseState.x + mouseState.dx, 0, maxW);
			mouseState.y = CLAMP(mouseState.y + mouseState.dy, 0, maxH);

			if (callback_mouse) callback_mouse(&mouseState);
		}


		/* Handle keyboard events */
		for (i = 0; i < _numberOfKeys; i++) {
			keyStates[i].pressed  = false;
			keyStates[i].released = false;
		}

		while(0 < read(hKb, &ev, sizeof(struct input_event))  ) {
			
			if (ev.type != EV_KEY) continue;

			EZ_KeyCode_t kcode = keyMap(ev.code);

			bool isPressed = ev.value > 0;

			keyStates[kcode].pressed  =  isPressed;
			keyStates[kcode].held     =  isPressed;
			keyStates[kcode].released = !isPressed;

			if (isPressed && callback_keyPressed) callback_keyPressed(&keyStates[kcode]);
			else if (!isPressed && callback_keyReleased) callback_keyReleased(&keyStates[kcode]);
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


	/* get back to text mode */
	ERR_assert(-1 != ioctl(hTerm, KDSETMODE, KD_TEXT),
		"Failed to get back to text mode");

	munmap(fbmem, buffSize);
	free(buffer);
	
	close(hFb);
	close(hTerm);
	close(hMouse);
	close(hKb);

	pthread_exit(NULL);

	return NULL;
}
