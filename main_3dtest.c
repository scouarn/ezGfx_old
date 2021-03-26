#include <ezGfx.h>
#include <ezSfx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

EZ_Image* test;

EZ_poly cubeMesh[12];
EZ_pcmArray tada;





void setup(void* param) {
    EZ_window("DEMO", 600, 600);
    canvas = EZ_createImage(200,200);

    EZ_draw2D_clear(canvas, EZ_BLUE);


    EZ_draw3D_unitCube(cubeMesh, EZ_WHITE);

    float fov = 3.141592 / 2.0f;
    float aRatio = 1.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;

    EZ_mat projection[4*4];
    EZ_draw3D_projTransform(projection, fov, aRatio, zNear, zFar);

    EZ_mat transform[3] = { -0.5f, -0.5f, 2.0f};

    EZ_draw3D_wireframe(canvas, cubeMesh, 12, projection, transform);


    //test = EZ_images_load("test.bin");
    //EZ_draw2D_image(canvas, test, 0, 0);


}


void draw(void* param) {
    //duration dt = *(double *)param;


    //EZ_draw2D_fillTri(canvas, EZ_WHITE , 50, 50, 75, 150, 125, 75);



}


void key(void* param) {

  for (int i = 0; i < _numberOfKeys; i++) {
    if (EZ_keyStates[i].pressed)
      printf("found key\n");
  }


}


void kill(void* param) {

  //EZ_images_save(canvas, "test.bin");
  //EZ_freeImage(test);
}




EZ_sample playRaw(double sampleTime, int channel) {

	return EZ_sfx_pcmNextSample(&tada);
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
