#include "ezGfx_core.h"
#include "ezGfx_utils.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <windows.h>
#include <Windowsx.h>
	

/* Application */
static DWORD  thread_id;
static HANDLE thread_handle;
static DWORD WINAPI mainThread(LPVOID arg);
static LRESULT CALLBACK windowProcedure( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HWND hwnd;
static HINSTANCE hInstance;


/* Drawing */
static HDC screenDC;
static HDC buffer;
static HBITMAP bmp = NULL;
static EZ_Image_t* canvas;
static int winWidth;
static int winHeight;
static struct {
	  BITMAPINFOHEADER bmiHeader;
	  DWORD mask[4];
} bmpinfo; /* https://devblogs.microsoft.com/oldnewthing/?p=28983 */

static EZ_Px_t* pixels;


/* Time */
static struct timespec startTime;
static struct timespec lastTime;
static volatile bool running;

/* Inputs */
static EZ_KeyCode_t keyMap(int keyCode);
static EZ_Key_t keyStates[_numberOfKeys];
static EZ_Mouse_t mouseState;



void EZ_bind(EZ_Image_t* cnvs) {
	canvas = cnvs;
	EZ_resize(canvas->w, canvas->h);

	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth  = canvas->w;
	bmpinfo.bmiHeader.biHeight = -canvas->h;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;
	bmpinfo.bmiHeader.biCompression = BI_BITFIELDS;
	bmpinfo.bmiHeader.biClrImportant = 0;
   	bmpinfo.bmiHeader.biClrUsed = 0;

  	bmpinfo.mask[0] = 0xff000000;
  	bmpinfo.mask[1] = 0x00ff0000;
  	bmpinfo.mask[2] = 0x0000ff00;

  	if (bmp) DeleteObject(bmp);

	bmp = CreateDIBSection(buffer, (BITMAPINFO*)&bmpinfo, DIB_RGB_COLORS, (void*)&pixels, NULL, 0);
	EZ_assert(bmp, "Couldn't init bitmap for buffer.");

}


void EZ_start() {
	running = true;

	/* start engine thread */
	thread_handle = CreateThread(NULL, 0, mainThread, NULL, 0, &thread_id);
		
}

void EZ_stop() {
	/* stop thread */
	running = false;
}

void EZ_join() {
	/* join thread */
	WaitForSingleObject(thread_handle, INFINITE);
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
	SetWindowTextA(hwnd, (LPCSTR)name);
}

void EZ_resize(int w, int h) {

	RECT rec = {0, 0, w, h};
	AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, false);

	rec.right -= rec.left;
	rec.bottom -= rec.top;

	SetWindowPos(hwnd, NULL, 0, 0, rec.right, rec.bottom, SWP_NOMOVE);

}

void EZ_setFullscreen(bool val) {
	

}

void EZ_setMaximized(bool val) {

	ShowWindow(hwnd, val ? SW_MAXIMIZE : SW_NORMAL);
	SetForegroundWindow(hwnd);
	UpdateWindow(hwnd);

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

	int x, y, cx, cy;

	float cvsAspct = canvas->w / canvas->h;
	float winAspct = winWidth / winHeight;

	if (cvsAspct / winAspct > 1) { /* horizontal bars */
		x = 0; cx = winWidth;
		cy = cx / cvsAspct;
		y = (winHeight - cy) / 2;
	}
	else { /* vertical bars */
		y = 0; 
		cy = winHeight;
		cx = cy * cvsAspct;
		x = (winWidth - cx) / 2;
	}



	/* draw bars */
	HPEN pen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
    SelectObject(screenDC, pen);

	HBRUSH brush = (HBRUSH)(COLOR_WINDOW+1);
	SelectObject(screenDC, brush);

	Rectangle(screenDC, 0, 0, x, winHeight); /* left */
	Rectangle(screenDC, 0, 0, winWidth, y);  /* top */
	Rectangle(screenDC, winWidth-x, 0, winWidth, winHeight);  /* right */
	Rectangle(screenDC, 0, winHeight-y, winWidth, winHeight);  /* bottom */

	DeleteObject(pen);
	DeleteObject(brush);

	/* copy texture to buffer */
	EZ_assert(
		SetDIBits(buffer, bmp, 0, canvas->h, canvas->px, (BITMAPINFO*)(&bmpinfo), DIB_RGB_COLORS),
		"Couldn't copy texture to buffer.");


	/* draw texture */
	SelectObject(buffer, bmp);
	EZ_assert(
		StretchBlt(screenDC, x, y, cx, cy,
		buffer, 0, 0, canvas->w, canvas->h,
		SRCCOPY),

		"Couldn't blit texture."
	);


}






/* PRIVATE */




static DWORD WINAPI mainThread(LPVOID arg) {


	/* init time */
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	clock_gettime(CLOCK_MONOTONIC, &lastTime);

	/* init keys */
	int i;
	for (i = 0; i < _numberOfKeys; i++)
		keyStates[i].keyCode = i;



	/* init window */
	/* https:docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window */
	/* https://gist.github.com/caiorss/e8967d4d3dad522c82aab18ccd8f8304 */
	
	/* Get WinMain Parameters */
	const char CLASSNAME [] = "EZGFXCLASS";
	WNDCLASSEX wc;

	hInstance = GetModuleHandle(NULL);

    /* register winclass */
	wc.lpszClassName = CLASSNAME;
	wc.lpfnWndProc = windowProcedure;	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style  = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;	
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);  

	EZ_assert(RegisterClassEx(&wc), "Couldn't register window class");
	
	/* create window */
	hwnd = CreateWindowA(
		CLASSNAME,
		"EZGFX",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 100, 150,
		NULL, NULL, hInstance, NULL	
	);

	EZ_assert(hwnd, "Couldn't create Window");

	/* init graphics context */
	screenDC = GetDC(hwnd);
	buffer = CreateCompatibleDC(screenDC);
	EZ_assert(buffer && screenDC, "Couldn't init device contexts.");


	/* client init callback */
	EZ_callback_init();

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	/* main loop */
	while (running) {


		/* elapsed time */
		struct timespec now;
		double elapsedTime;
		clock_gettime(CLOCK_MONOTONIC, &now);
		elapsedTime = (now.tv_sec - lastTime.tv_sec) + (now.tv_nsec - lastTime.tv_nsec) / 1000000000.0;
		lastTime = now;

		/* user function */
		EZ_callback_draw(elapsedTime);


		/* update keystates */
		int i;
		for (i = 0; i < _numberOfKeys; i++) {
			keyStates[i].pressed  = false;
			keyStates[i].released = false;
		}
		mouseState.wheel = 0;

		/* events and inputs */
		MSG Msg;
		while(PeekMessage(&Msg, hwnd, 0, 0, PM_REMOVE)){
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}	

		/* display */
		if (canvas) EZ_redraw();


	}

	EZ_callback_kill();
		
	DeleteObject(bmp);
	DeleteDC(buffer);
	DeleteDC(screenDC);
	CloseHandle(thread_handle);

	return 0;
}


#define MAPTO(X, Y) case X : return Y;

static EZ_KeyCode_t keyMap(int scancode) {
	switch (scancode) {
		MAPTO(VK_LBUTTON, K_LMB) MAPTO(VK_RBUTTON, K_RMB) MAPTO(VK_MBUTTON, K_MMB)

		MAPTO(VK_TAB, K_TAB)        MAPTO(VK_ESCAPE, K_ESCAPE) MAPTO(VK_SPACE, K_SPACE)
		MAPTO(VK_BACK, K_BACKSPACE)	MAPTO(VK_RETURN, K_RETURN)

		MAPTO(VK_PRIOR, K_PGUP) MAPTO(VK_NEXT, K_PGDN)
		MAPTO(VK_END, K_END)    MAPTO(VK_HOME, K_HOME)

		MAPTO(VK_LEFT, K_LEFT)   MAPTO(VK_UP, K_UP)
		MAPTO(VK_RIGHT, K_RIGHT) MAPTO(VK_DOWN, K_DOWN)

		MAPTO(VK_INSERT, K_INS)      MAPTO(VK_DELETE, K_DEL)	MAPTO(VK_SCROLL, K_SCROLL)
		MAPTO(VK_NUMLOCK, K_NUMLOCK) MAPTO(VK_PAUSE, K_PAUSE)

		MAPTO(VK_MULTIPLY, KP_MUL)	MAPTO(VK_ADD, KP_PLUS)  MAPTO(VK_SUBTRACT, KP_MINUS)
		MAPTO(VK_DECIMAL, KP_DEC)   MAPTO(VK_DIVIDE, KP_DIV)

		MAPTO(VK_CAPITAL, K_CAPS)
		MAPTO(VK_LSHIFT, K_LSHIFT)  MAPTO(VK_RSHIFT, K_RSHIFT)
		MAPTO(VK_LCONTROL, K_LCTRL)	MAPTO(VK_RCONTROL, K_RCTRL)
		MAPTO(VK_LMENU, K_LALT)     MAPTO(VK_RMENU, K_RALT)

		MAPTO(VK_OEM_1, K_COLON)     MAPTO(VK_OEM_2, K_SLASH)
		MAPTO(VK_OEM_3, K_TILDE)     MAPTO(VK_OEM_4, K_OPEN)
		MAPTO(VK_OEM_5, K_BACKSLASH) MAPTO(VK_OEM_6, K_CLOSE)
		MAPTO(VK_OEM_7, K_QUOTE)     MAPTO(VK_OEM_8, K_SLASH)
		MAPTO(VK_OEM_PLUS, K_PLUS)   MAPTO(VK_OEM_COMMA, K_COMMA)
		MAPTO(VK_OEM_MINUS, K_MINUS) MAPTO(VK_OEM_PERIOD, K_PERIOD)

		case 0x30 ... 0x39 : return scancode - 0x30 + K_0;
		case 0x41 ... 0x5A : return scancode - 0x41 + K_A;
		case VK_NUMPAD0...VK_NUMPAD9 : return scancode - VK_NUMPAD0 + KP_0;
		case VK_F1...VK_F12 : return scancode - VK_F1 + K_F1;

		default : return K_ERROR;
	}
}


static void kdown(EZ_Key_t* key) {

	key->pressed = true;
	key->held    = true;

	EZ_callback_keyPressed(key);
}

static void kup(EZ_Key_t* key) {

	key->released = true;
	key->held = false;

	EZ_callback_keyReleased(key);
}



/* event handler */
static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg)	{

	case WM_CREATE:

	break;

    case WM_CLOSE:		
		EZ_stop();
		DestroyWindow(hwnd);
	break;

    case WM_DESTROY:
    	EZ_stop();
		PostQuitMessage(0);
	break;

	case WM_MOVE:
		
	break; 

	case WM_SIZE:
		winWidth  = LOWORD(lParam);
		winHeight = HIWORD(lParam);
	break;


	case WM_KEYDOWN: kdown( &keyStates[keyMap(wParam)] ); break;

	case WM_KEYUP: kup( &keyStates[keyMap(wParam)] ); break;

	case WM_LBUTTONDOWN : kdown( &keyStates[K_LMB] ); break;
	case WM_MBUTTONDOWN : kdown( &keyStates[K_MMB] ); break;
	case WM_RBUTTONDOWN : kdown( &keyStates[K_RMB] ); break;

	case WM_LBUTTONUP : kup( &keyStates[K_LMB] ); break;
	case WM_MBUTTONUP : kup( &keyStates[K_MMB] ); break;
	case WM_RBUTTONUP : kup( &keyStates[K_RMB] ); break;


	case WM_MOUSEMOVE :
		mouseState.dx = GET_X_LPARAM(lParam) - mouseState.x;
		mouseState.dy = GET_Y_LPARAM(lParam) - mouseState.y;

		mouseState.x += mouseState.dx;
		mouseState.y += mouseState.dy;

		EZ_callback_mouseMoved(&mouseState);
	break;

	case WM_MOUSEWHEEL :
		mouseState.wheel = GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? -1 : 1;
		EZ_callback_mouseMoved(&mouseState);
	break;

    default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
    }

	return 0;
}
