#include "ezGfx_core.h"
#include "ezGfx_draw2D.h"
#include "ezGfx_matrix.h"

#include <stdlib.h>
#include <stdio.h>


void setup(void* param) {
    EZ_window("DEMO", 600, 600);
    canvas = EZ_createImage(100,100);

}

void draw(void* param) {
    duration dt = *(double *)param;

    EZ_draw2D_clear(canvas, EZ_BLUE);
    EZ_draw2D_rect (canvas, (EZ_px)0xFFFFFF88U, 10,10,15,15);


    EZ_draw2D_fillTri(canvas, (EZ_px)0xFFFFFF88U, 10, 75, 30, 80, 20, 60);
    EZ_draw2D_tri(canvas, EZ_RED, 10, 75, 30, 80, 20, 60);
    EZ_draw2D_fillElli(canvas, EZ_WHITE, 70, 70, 25, 15);
    EZ_draw2D_line (canvas, EZ_WHITE, 50, 50, EZ_mouse.x, EZ_mouse.y);

}

void mouse(void* param) {

}

void kill(void* param) {

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
