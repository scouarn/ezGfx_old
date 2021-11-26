#include "ezGfx_core.h"
#include "ezGfx_utils.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <windows.h>


/* Application */
static DWORD  thread_id;
static HANDLE thread_handle;
static DWORD WINAPI mainThread(LPVOID arg);
static LRESULT CALLBACK windowProcedure( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* Graphic context */



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

/* Inputs */
static EZ_KeyCode_t keyMap(int keyCode);
static EZ_Key_t keyStates[_numberOfKeys];
static EZ_Mouse_t mouseState;



void EZ_bind(EZ_Image_t* cnvs) {
	canvas = cnvs;
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
	
}

void EZ_resize(int w, int h) {
	/* RESIZE WINDOW */
	winWidth = w;
	winHeight = h;
}

void EZ_setFullscreen(bool val) {
	

}

void EZ_setMaximized(bool val) {
	

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

	/* draw the buffer on the screen */


}






/* PRIVATE */





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


static DWORD WINAPI mainThread(LPVOID arg) {
	int i;

	STARTUPINFO si;
	int nCmdShow;
	HINSTANCE hInstance;
	const char CLASSNAME [] = "EZGFXCLASS";
	WNDCLASSEX wc;
	MSG Msg;
	HWND hwnd;

	/* init time */
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	clock_gettime(CLOCK_MONOTONIC, &lastTime);

	/* init keys */
	for (i = 0; i < _numberOfKeys; i++)
		keyStates[i].keyCode = i;



	/* init window */
	/* https:docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window */
	/* https://gist.github.com/caiorss/e8967d4d3dad522c82aab18ccd8f8304 */
	
	/* Get WinMain Parameters */
	hInstance = GetModuleHandle(NULL);
    GetStartupInfo(&si);  
    nCmdShow = si.wShowWindow;

		
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
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL	
	);

	EZ_assert(hwnd, "Couldn't create Window");


	/* client init callback */
	EZ_callback_init();
	if (canvas->px != NULL) EZ_resize(canvas->w, canvas->h);
	_updateBars();


	/* start of the loop */
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	/* handle new messages */
	while(GetMessage(&Msg, NULL, 0, 0) > 0 ){
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}	
	



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
		for (i = 0; i < _numberOfKeys; i++) {
			keyStates[i].pressed  = false;
			keyStates[i].released = false;
		}

		/* events and inputs */
		

		/* display */
		EZ_redraw();


	}

	EZ_callback_kill();
	CloseHandle(thread_handle);
	return 0;
}




static EZ_KeyCode_t keyMap(int keyCode) {
	switch (keyCode) {
		default : return K_ERROR;
	}
}


/* event handler */
static LRESULT CALLBACK windowProcedure (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

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
	
	case WM_PAINT:	{
		PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);


        EndPaint(hwnd, &ps);
	}
	break;

    default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
    }

	return 0;
}
