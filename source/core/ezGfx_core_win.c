#include "ezGfx_core_common.h"
#include <ctype.h>

#include <windows.h>
#include <Windowsx.h>


/* Application */
static DWORD  thread_id;
static HANDLE thread_handle;
static DWORD WINAPI mainThread(LPVOID arg);
static LRESULT CALLBACK windowProcedure( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HWND hwnd;
static HINSTANCE hInstance;


/* Gfx context */
static HDC screenDC;
static HDC buffer;
static HBITMAP bmp = NULL;
static EZ_Px_t* pixels; /* not a buffer just a pointer to what microsoft allocated to me */
static struct {
	  BITMAPINFOHEADER bmiHeader;
	  DWORD mask[4];
} bmpinfo; /* https://devblogs.microsoft.com/oldnewthing/?p=28983 */



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



void EZ_redraw() {

	__rect__ rec;
	getCanvasRect(&rec);


	/* draw bars */
	HPEN pen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
    SelectObject(screenDC, pen);

	HBRUSH brush = (HBRUSH)(COLOR_WINDOW+1);
	SelectObject(screenDC, brush);

	Rectangle(screenDC, 0, 0, rec.x1, winHeight); /* left */
	Rectangle(screenDC, 0, 0, winWidth, rec.y1);  /* top */
	Rectangle(screenDC, rec.x2, 0, winWidth, winHeight);  /* right */
	Rectangle(screenDC, 0, rec.y2, winWidth, winHeight);  /* bottom */

	DeleteObject(pen);
	DeleteObject(brush);

	/* copy texture to buffer */
	EZ_assert(
		SetDIBits(buffer, bmp, 0, canvas->h, canvas->px, (BITMAPINFO*)(&bmpinfo), DIB_RGB_COLORS),
		"Couldn't copy texture to buffer.");


	/* draw texture */
	SelectObject(buffer, bmp);
	EZ_assert(
		StretchBlt(screenDC, rec.x1, rec.y1, rec.cx, rec.cy,
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
	if (callback_init) callback_init();

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
		if (callback_draw) callback_draw(elapsedTime);


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
		if (canvas && hwnd == GetActiveWindow()) /* if not active redraw will crash the program */
			EZ_redraw();

	}

	if (callback_kill) callback_kill();
		
	DeleteObject(bmp);
	DeleteDC(buffer);
	DeleteDC(screenDC);
	CloseHandle(thread_handle);

	return 0;
}


static EZ_KeyCode_t keyMap(int scancode) {
	#define MAPTO(X, Y) case X : return Y;
	
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
		MAPTO(VK_SHIFT, K_LSHIFT)
		MAPTO(VK_CONTROL, K_LCTRL)
		MAPTO(VK_MENU, K_LALT)

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

	#undef MAPTO
}


static void updateKey(int scancode, int param, bool down) {

	EZ_KeyCode_t code = keyMap(scancode);
	if (!code) return;

	EZ_Key_t* key = keyStates + code;

	/* get the typed character */
	if (keyStates[K_LSHIFT].held || keyStates[K_RSHIFT].held || (GetKeyState(VK_CAPITAL) & 0x0001)) {
		
		key->typed = MapVirtualKeyA(scancode, MAPVK_VK_TO_CHAR);
	}
	else {
		key->typed = tolower( MapVirtualKeyA(scancode, MAPVK_VK_TO_CHAR) );
	}


	if (down) {

		key->pressed = true;
		key->held    = true;

		if (callback_keyPressed && key->keyCode) callback_keyPressed(key);
	}
	else {
		key->released = true;
		key->held = false;

		if (callback_keyReleased && key->keyCode) callback_keyReleased(key);
	}	


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

	case WM_SYSKEYDOWN: updateKey(wParam, lParam, 1); break;
	case WM_SYSKEYUP:   updateKey(wParam, lParam, 0); break;
	
	case WM_KEYDOWN: updateKey(wParam, lParam, 1); break;
	case WM_KEYUP:   updateKey(wParam, lParam, 0); break;

	case WM_LBUTTONDOWN : updateKey(VK_LBUTTON, lParam, 1); break;
	case WM_MBUTTONDOWN : updateKey(VK_MBUTTON, lParam, 1); break;
	case WM_RBUTTONDOWN : updateKey(VK_RBUTTON, lParam, 1); break;

	case WM_LBUTTONUP : updateKey(VK_LBUTTON, lParam, 0); break;
	case WM_MBUTTONUP : updateKey(VK_MBUTTON, lParam, 0); break;
	case WM_RBUTTONUP : updateKey(VK_RBUTTON, lParam, 0); break;


	case WM_MOUSEMOVE : {


		if (!canvas) break;

		__rect__ rec;
		getCanvasRect(&rec);

		int winX = GET_X_LPARAM(lParam);
		int winY = GET_Y_LPARAM(lParam);

		int newX = (winX - rec.x1) * canvas->w / rec.cx;
		int newY = (winY - rec.y1) * canvas->h / rec.cy;

		if (newX >= 0 && newX < canvas->w
		 && newY >= 0 && newY < canvas->h) {
			mouseState.x  = newX;
			mouseState.y  = newY;
			mouseState.dx = newX - mouseState.x;
			mouseState.dy = newY - mouseState.y;


			if (callback_mouse) callback_mouse(&mouseState);
		}
	}
	break;

	case WM_MOUSEWHEEL :
		mouseState.wheel = GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? -1 : 1;
		if (callback_mouse) callback_mouse(&mouseState);
	break;

    default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
    }

	return 0;
}
