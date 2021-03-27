#include <ezGfx.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>



void setup(void* param) {
    EZ_window("DEMO", 600, 600);
    canvas = EZ_createImage(200,200);

    EZ_draw2D_clear(canvas, EZ_BLUE);




}


void draw(void* param) {
    //duration dt = *(double *)param;

}


EZ_sfx_note basicSine = {0};
double pitchTable[12] = {110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65};

void key(void* param) {
  int lastPressed = (int)param;
  int index = lastPressed - K_F1;

  printf("index %d\n",index);
  double octave = 2.0f;


  basicSine.callback = &EZ_sfx_fastSine;
  basicSine.length = 1.0;
  basicSine.pitch = pitchTable[index] * octave;
  EZ_sfx_play(basicSine);

}

void keyRelease(void* param) {

}

void mouse(void* param) {


}

void kill(void* param) {

}



int main (int argc, char **argv) {

    EZ_callbacks[ON_CREATE] = &setup;
    EZ_callbacks[ON_DRAW]   = &draw;
    EZ_callbacks[ON_CLOSE]  = &kill;
    EZ_callbacks[ON_MOUSEMOVE]  = &mouse;
    EZ_callbacks[ON_KEYPRESSED] = &key;
    EZ_callbacks[ON_KEYRELEASED] = &keyRelease;

    EZ_sfx_init(CD41KHZ, 2, 16, 512, &EZ_sfx_plusCallback);

    EZ_start();
    EZ_sfx_start();
    EZ_join();


    EZ_sfx_stop();


    return 0;
}
