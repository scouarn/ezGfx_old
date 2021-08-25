#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


EZ_PCMArray sound;


void EZ_callback_init() {}
void EZ_callback_draw(double dt) {}
void EZ_callback_keyPressed(EZ_Key key) {}
void EZ_callback_keyReleased(EZ_Key key) {}
void EZ_callback_mouseMoved(EZ_Mouse mouse) {}
void EZ_callback_kill() {}

EZ_Sample EZ_sfx_callback(double time, int channel) {

  	return EZ_sfx_pcmNextSample(&sound);
}





int main (int argc, char **argv) {

	printf("%s\n",argv[0]);


	sound = EZ_load_WAV("warpigs.wav");

	EZ_sfx_init(CD44KHZ, 2, 16, 512);

	EZ_start();
	EZ_sfx_start();
	EZ_sfx_join();
	EZ_stop();

	EZ_sfx_pcmFree(sound);

	return 0;
}
