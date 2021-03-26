#include <ezGfx.h>
#include <ezSfx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


EZ_pcmArray tada;


void setup(void* param) {
    EZ_window("DEMO", 600, 600);
    canvas = EZ_createImage(200,200);

    EZ_draw2D_clear(canvas, EZ_BLUE);

}


void draw(void* param) {
    //duration dt = *(double *)param;

}


void key(void* param) {

  for (int i = 0; i < _numberOfKeys; i++) {
    if (EZ_keyStates[i].pressed)
      printf("found key\n");
  }


}

void kill(void* param) {

}


double sineOscillator(double time, double freq) {
	return sin(time*freq*3.141592*2.0);

}


EZ_sample sampleClbk(double time, int channel) {

  double sf = 0;



  if (EZ_keyStates[K_A].held)
    sf += sineOscillator(time, 440.0);

  //mix


  //
  return (EZ_sample)(sf*SAMPLE_MAX);
}



int main (int argc, char **argv) {

    EZ_callbacks[ON_CREATE] = &setup;
    EZ_callbacks[ON_DRAW]   = &draw;
    EZ_callbacks[ON_CLOSE]  = &kill;
    //EZ_callbacks[ON_MOUSEMOVE] = &mouse;
    EZ_callbacks[ON_KEYPRESSED] = &key;

    EZ_sfx_init(CD41KHZ, 2, 16, 512, &sampleClbk);
    tada = EZ_sfx_pcmLoad("audio/war.raw");

    EZ_start();
    EZ_sfx_start();
    EZ_join();


    EZ_sfx_stop();

    EZ_sfx_pcmFree(&tada);

    return 0;
}
