#include "ezGfx_core_common.h"

#include <pthread.h>
#include <unistd.h>

#include <linux/fb.h>	/* linux frame buffer */
#include <linux/kd.h>  	/* terminal type/mode */
#include <sys/ioctl.h>	/* system calls */
#include <sys/mman.h>	/* MAP_ flags */
#include <fcntl.h>		/* open device */
#include <unistd.h>		/* close device */
#include <signal.h>		/* failsafe */


/* App */
static pthread_t thread;
static void* mainThread(void* arg);
static int hTerm;

/* Graphic context */
static char* fbmem;
static char* buffer;
static int fbfile;
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
	   and cannot achieve more than 30 fps on my machine */
	memcpy(fbmem, buffer, winWidth * winHeight * 4);
}







static EZ_KeyCode_t keyMap(int keyCode) {

	switch (keyCode) {
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


	/* open frame buffer device */
	ERR_assert(-1 != (fbfile = open("/dev/fb0", O_RDWR)), 
		"Couldn't open /dev/fb0,\n" 
		"maybe add yourself in video group");

	/* get screen info */
	ERR_assert(-1 != ioctl(fbfile, FBIOGET_VSCREENINFO, &screenInfo),
		"Couldn't get screen info");
	
	ERR_assert(screenInfo.bits_per_pixel == 32, 
		"Unsupported bits pixel format");

	winWidth  = screenInfo.xres; 
	winHeight = screenInfo.yres;

	/* get handle to screen buffer */
	size_t buffSize = 4 * screenInfo.xres * screenInfo.yres;
	fbmem = mmap(NULL, buffSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfile, 0);

	ERR_assert(fbmem != MAP_FAILED, 
		"Couldn't map the frame buffer");


	/* init buffers */
	memset(fbmem, 0x00, buffSize);
	buffer = calloc(buffSize, 1);

	/* open the console device */
	ERR_assert(-1 != (hTerm = open("/dev/console", O_NOCTTY)),
		"Failed to open console device /dev/console,\n"
		"maybe try running as root.");

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

	/* init keys */
	for (i = 0; i < _numberOfKeys; i++)
		keyStates[i].code = i;

	/* client init callback */
	if (callback_init) callback_init();



	/* main loop */
	while (running) {

		/* time control */
		double loopBegin = EZ_getTime();


		/* update keystates */
		for (i = 0; i < _numberOfKeys; i++) {
			keyStates[i].pressed  = false;
			keyStates[i].released = false;
		}
		mouseState.wheel = 0;

		/* events and inputs */

			/******/
			/******/
			/******/


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
	close(fbfile);
	close(hTerm);

	pthread_exit(NULL);

	return NULL;
}
