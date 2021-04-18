#include <ezGfx.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

EZ_Image* canvas;

void setup(void* param) {
    canvas = EZ_createImage(200,200);
    EZ_window("DEMO", 600, 600, canvas);


    EZ_draw2D_clear(canvas, EZ_BLUE);


}


void draw(void* param) {

}

void key(void* param) {
  
}

void keyRelease(void* param) {

}

void mouse(void* param) {


}

void kill(void* param) {
  EZ_freeImage(canvas);
}



int main (int argc, char **argv) {

    EZ_setCallbak(ON_CREATE, &setup);
    EZ_setCallbak(ON_DRAW,   &draw);
    EZ_setCallbak(ON_CLOSE,  &kill);
    EZ_setCallbak(ON_MOUSEMOVE,   &mouse);
    EZ_setCallbak(ON_KEYPRESSED,  &key);
    EZ_setCallbak(ON_KEYRELEASED, &keyRelease);

    EZ_start();
    EZ_join();


    return 0;
}
