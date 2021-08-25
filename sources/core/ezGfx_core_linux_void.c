#include "ezGfx_core.h"


#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include <linux/fb.h>		//linux frame buffer
#include <linux/kd.h>   	//terminal type/mode
#include <sys/ioctl.h>		//system calls
#include <sys/mman.h>		//MAP_ flags
#include <signal.h> 		//interrupt handling
#include <fcntl.h>			//open device
#include <unistd.h>			//close device



static struct timespec startTime;
static struct timespec lastTime;
static volatile bool running;
static pthread_t thread;
static void* mainThread(void* arg);

static EZ_Image canvas;		
static int winWidth;
static int winHeight;
static float canvasX = 1.0f;
static float canvasY = 1.0f;


static char* frame_buffer; 	//memory "inside" the screen
static char* buffer;	   	//memory "ouside" the screen
static int fb;				//frame buffer handle
static int vt;				//terminal handle
static struct fb_var_screeninfo screenInfo;


// static int keyMap(int keyCode);
static EZ_Key keyStates[_numberOfKeys];
static EZ_Mouse mouseState;
static void updateBars();


static void __int_handler__(int sig) {EZ_stop();}


void EZ_bind(EZ_Image cnvs) {
	canvas = cnvs;
}

void EZ_start() {
	running = true;
	pthread_create(&thread, NULL, &mainThread, NULL);
}

void EZ_stop() {
	running = false;
}

void EZ_join() {
	pthread_join(thread, NULL);
}

double EZ_getTime() {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	double time = (now.tv_sec - startTime.tv_sec) + (now.tv_nsec - startTime.tv_nsec) / 1000000000.0;

	return time;
}

EZ_Key EZ_getKey(enum EZ_KeyCode code) {
	return keyStates[code];
}

EZ_Mouse EZ_getMouse() {
	return mouseState;
}


void EZ_rename(const char* name) {
	WARNING("Unsupported by this implementation");
}

void EZ_resize(int w, int h) {
	WARNING("Unsupported by this implementation");
}

void EZ_setFullscreen(bool val) {
	WARNING("Unsupported by this implementation");
}

void EZ_setMaximized(bool val) {
	WARNING("Unsupported by this implementation");
}


EZ_Image EZ_createImage(int w, int h) {
	EZ_Image img = {0};
	img.w = w;
	img.h = h;
	img.px = calloc(w*h, sizeof(EZ_Px));

	return img;
}

EZ_Image EZ_copyImage(EZ_Image source) {

	EZ_Image img = {0};
	img.w = source.w;
	img.h = source.h;

	img.px = malloc(img.w*img.h*sizeof(EZ_Px));
	memcpy(img.px, source.px, img.w*img.h*sizeof(EZ_Px));

	return img;
}

void EZ_freeImage(EZ_Image img) {
	if (img.px != NULL) free(img.px);
}



EZ_Font EZ_createFont(int w, int h) {

	EZ_Font font = {0};
	font.h_px = h; font.w_px = w;
	font.w_bytes = w/9 + 1;

	//allocate for 256 pointer
	font.data = malloc(256*sizeof(char*));

	//allocate for 256 glyphs
	font.data[0] = calloc(256*font.w_bytes*h, 1);

	if (font.data[0] == NULL) {
		WARNING("Couldn't allocate memory for font\n");
		font.data = NULL;
		return font;
	}

	//point each pointer to the desired location
	for (int i = 1; i < 256; i++) font.data[i] = font.data[i-1] + font.w_bytes*h;

	return font;
}



void EZ_freeFont(EZ_Font font) {
	if (font.data[0]) free(font.data[0]);
	if (font.data)    free(font.data);
}



EZ_Px EZ_randCol() {
	EZ_Px col;
	col.ref = (long)rand() << 8;
	return col;
}

EZ_Px EZ_blend(EZ_Px fg, EZ_Px bg, enum EZ_BlendMode mode) {
	//https://love2d.org/wiki/BlendMode_Formulas
	EZ_Px result;

	switch (mode) {
		case ALPHA_IGNORE : result = fg; break;
		case ALPHA_FAST   : result = (fg.col.a == 0) ? bg : fg; break;

		default :
		case ALPHA_BLEND  : 
		
		//integer math only	optimization
		result.col.a = fg.col.a + (255 - fg.col.a) * bg.col.a;
		result.col.r = (fg.col.a * fg.col.r + (255 - fg.col.a) * bg.col.r) >> 8;
		result.col.g = (fg.col.a * fg.col.g + (255 - fg.col.a) * bg.col.g) >> 8;
		result.col.b = (fg.col.a * fg.col.b + (255 - fg.col.a) * bg.col.b) >> 8;


		break;
	}


	return result;
}


void EZ_redraw() {

	
	//copy canvas directly to frame buffer
	int x1 = winWidth * (1.0f-canvasX) * 0.5f;
	int x2 = x1 + winWidth * canvasX;

	int y1 = winHeight * (1.0f - canvasY) * 0.5f;
	int y2 = y1 + winHeight * canvasY;

	float xRatio = (float)canvas.w / (x2-x1);
	float yRatio = (float)canvas.h / (y2-y1);

	for (int x = x1; x < x2; x++) 
	for (int y = y1; y < y2; y++) {

		int sx = (x-x1)*xRatio;
		int sy = (y-y1)*yRatio;

		EZ_Px px = canvas.px[sx + sy*canvas.w];

		buffer[(x + y*winWidth)*4   ] = px.col.b;
		buffer[(x + y*winWidth)*4 +1] = px.col.g;
		buffer[(x + y*winWidth)*4 +2] = px.col.r;

	}

	//copy to screen 
	//really faster than directly writing to frame buffer
	//but I don't absolutely know why, physical location of the buffer ?
	memcpy(frame_buffer, buffer, 4*winWidth*winHeight);

}






/* PRIVATE */

static void updateBars() {

	//black bars

	float canvasRatio = (float)canvas.w / canvas.h;
	float windowRatio = (float)winWidth / winHeight;
	canvasX = canvasRatio / windowRatio;
	canvasY = windowRatio / canvasRatio;

	if (canvasX > 1.0f)
		canvasX = 1.0f;

	else
		canvasY = 1.0f;


}



static void* mainThread(void* arg) {

	pthread_detach(pthread_self());

	//INIT GFX
	//https://docs.huihoo.com/doxygen/linux/kernel/3.7/structfb__var__screeninfo.html
	//https://cmcenroe.me/2018/01/30/fbclock.html


	//open the linux frame buffer device directly 
	ASSERTM(-1 != (fb = open("/dev/fb0", O_RDWR)), 
		"Couldn't open /dev/fb0 frame buffer,\n" 
		"maybe add yourself in video group");

	//get screen info
	ASSERTM(-1 != ioctl(fb, FBIOGET_VSCREENINFO, &screenInfo),
		"Couldn't get screen info");
	
	winWidth  = screenInfo.xres; 
	winHeight = screenInfo.yres;

	//get handle to screen buffer
	size_t len = 4 * screenInfo.xres * screenInfo.yres;
	frame_buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);

	ASSERTM(buffer != MAP_FAILED, 
		"Couldn't map the frame buffer");

	ASSERTM(screenInfo.bits_per_pixel == 32, 
		"Unsupported %dbits pixel format", screenInfo.bits_per_pixel);


	//override keyboard interrupt to set graphics mode back
	signal(SIGINT, __int_handler__); 


	//open the console device
	ASSERTM(-1 != (vt = open("/dev/console", O_NOCTTY)),
		"Failed to open console device /dev/console,\n"
		"maybe try running as root.");

	//switch to graphics mode
	ASSERTM(-1 != ioctl(vt, KDSETMODE, KD_GRAPHICS),
		"Failed to get back to text mode");


	//clear buffer
	for (size_t i = 0; i < len; i++) frame_buffer[i] = 0x00;

	//setup offscreen buffer
	buffer = calloc(winWidth * winHeight, 4);


	//init time
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	clock_gettime(CLOCK_MONOTONIC, &lastTime);

	//init keys
	for (int i = 0; i < _numberOfKeys; i++)
		keyStates[i].keyCode = i;

	//client init callback
	EZ_callback_init();

	//canvas display stretching variables
	updateBars();




	//main loop
	while (running) {

		//time
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		double elapsedTime = (now.tv_sec - lastTime.tv_sec) + (now.tv_nsec - lastTime.tv_nsec) / 1000000000.0;
		lastTime = now;

		//user function
		EZ_callback_draw(elapsedTime);


		//TODO events, inputs
		for (int i = 0; i < _numberOfKeys; i++) {
			keyStates[i].pressed  = false;
			keyStates[i].released = false;


			//check the input buffer ?


		}



		//display
		EZ_redraw();


	}

	//call on kill user function
	EZ_callback_kill();

	//get back to text mode
	ASSERTM(-1 != ioctl(vt, KDSETMODE, KD_TEXT),
		"Failed to get back to text mode");


	//garbage collection
	munmap(frame_buffer, 4 * winHeight * winWidth);
	free(buffer);
	close(fb);
	close(vt);
	pthread_exit(NULL);

	return NULL;
}
