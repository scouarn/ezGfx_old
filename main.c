#include <ezGfx.h>
#include <stdlib.h>
#include <stdio.h>

EZ_Image* test;

void setup(void* param) {
    EZ_window("DEMO", 600, 600);
    canvas = EZ_createImage(200,200);
    //test = EZ_images_load("test.bin");
    //EZ_draw2D_image(canvas, test, 0, 0);
}

void draw(void* param) {
    duration dt = *(double *)param;

    EZ_draw2D_fillTri(canvas, EZ_WHITE, 50, 50, 150, 150, 100, 75);
}

void mouse(void* param) {

}

void kill(void* param) {
  //EZ_images_save(canvas, "test.bin");
  EZ_freeImage(test);
}




int main (int argc, char **argv)
{
    EZ_callbacks[ON_CREATE] = &setup;
    EZ_callbacks[ON_DRAW]   = &draw;
    EZ_callbacks[ON_CLOSE]  = &kill;
    EZ_callbacks[ON_MOUSEMOVE] = &mouse;

    EZ_start();
    EZ_join();

    return 0;
}
