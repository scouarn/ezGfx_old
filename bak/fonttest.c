#include <ezGfx.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

EZ_Image canvas;
EZ_Font  font;
char fname[256];


void setup(void* param) {
    canvas = EZ_createImage(256,256);
    EZ_window("DEMO", 600, 600, canvas);


    EZ_draw2D_clear(canvas, EZ_BLUE);



    printf("Loading font %s\n", fname);
    font = EZ_fonts_load(fname);

    char* ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Euismod elementum nisi quis eleifend quam adipiscing vitae proin. Euismod elementum nisi quis eleifend quam adipiscing vitae proin sagittis. Vulputate sapien nec sagittis aliquam malesuada bibendum arcu vitae elementum. At urna condimentum mattis pellentesque id nibh tortor id. Massa tincidunt nunc pulvinar sapien et ligula. Viverra justo nec ultrices dui sapien eget. Ut tortor pretium viverra suspendisse. Sed viverra tellus in hac habitasse platea dictumst vestibulum rhoncus. Eget aliquet nibh praesent tristique magna sit. Dui sapien eget mi proin sed libero enim sed. Suspendisse sed nisi lacus sed viverra tellus. Sit amet consectetur adipiscing elit. Vel risus commodo viverra maecenas accumsan. In nibh mauris cursus mattis molestie a iaculis at. Ante metus dictum at tempor commodo ullamcorper a lacus vestibulum. Augue ut lectus arcu bibendum at varius vel pharetra. Magna eget est lorem ipsum dolor sit. Et malesuada fames ac turpis egestas.";

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            char c = ipsum[x+y*32];
            EZ_fonts_printChar(canvas, c, font, EZ_WHITE, x*8, y*8);
        }
    }

    EZ_draw2D_saveBMP(canvas, "./images/ipsum.bmp");
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
  EZ_fonts_free(font);
}



int main (int argc, char **argv) {

    if (argc == 1) {
            printf("No file\n");
            exit(EXIT_FAILURE);
        }

        else {
            strcpy(fname, argv[1]);
        }

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
