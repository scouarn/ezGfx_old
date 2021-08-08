#include <ezGfx.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EZ_Image bitmap;

char fname[256];



void setup(void* param) {




    printf("Loading %s\n", fname);


    //open file
    FILE *file = fopen(fname,"rb");

    //check if opened
    if (file == NULL) {
        printf("Couldn't load file.");
    }

    else {
        
        char buffer[64]; //for garbage
        #define POP(n) fread(&buffer, 1, n, file)

        uint32_t fsize;
        uint32_t offset;

        //read file header (fixed 14 bytes)
        POP(2);
        fread(&fsize, 4, 1, file);
        POP(4);
        fread(&offset, 4, 1, file);


        printf("File size : %d\n", fsize);
        printf("Offset : %d\n", offset);

        
        //read DIB header
        
        uint32_t header_size;
        uint32_t image_size;
        uint16_t depth;

        uint32_t r_mask;
        uint32_t g_mask;
        uint32_t b_mask;
        uint32_t a_mask;

        fread(&header_size, 4, 1, file);
        fread(&bitmap.w, 4, 1, file);
        fread(&bitmap.h, 4, 1, file);
        POP(2); //planes
        fread(&depth, 2, 1, file);
        int bytes_ppx = depth / 8;

        POP(4); //compression
        fread(&image_size, 4, 1, file);
        POP(16); //resolution and color map info

        fread(&r_mask, 4, 1, file);
        fread(&g_mask, 4, 1, file);
        fread(&b_mask, 4, 1, file);
        fread(&a_mask, 4, 1, file);


        printf("Header size : %d\n", header_size);
        printf("Width : %d\n", bitmap.w);
        printf("Height : %d\n", bitmap.h);
        printf("Depth : %d\n", depth);
        printf("Image size : %d\n", image_size);

        printf("R mask : %08x\n", r_mask);
        printf("G mask : %08x\n", g_mask);
        printf("B mask : %08x\n", b_mask);
        printf("A mask : %08x\n", a_mask);


        
        //read pixels

        bitmap.px = calloc(bitmap.w*bitmap.h, sizeof(EZ_px));
        fseek(file, offset, SEEK_SET); //offset from begining

        int bytes_per_line = image_size / bitmap.h;

        uint8_t line[bytes_per_line]; //buffer

        for (int y = bitmap.h-1; y >= 0; y--) { //y axis reversed in bitmap format
            fread(line, 1, bytes_per_line, file);

            for (int x = 0; x < bitmap.w; x++) {

                EZ_px* px = &bitmap.px[x+y*bitmap.w];

                //test each color byte against each byte of each bitmask
                for (int c = 0; c < bytes_ppx; c++) {
                    px->col.r |= line[x*bytes_ppx + c] & (r_mask >> 8*c);
                    px->col.g |= line[x*bytes_ppx + c] & (g_mask >> 8*c);
                    px->col.b |= line[x*bytes_ppx + c] & (b_mask >> 8*c);
                    px->col.a |= line[x*bytes_ppx + c] & (a_mask >> 8*c);
                }


            }
        }


        //close
        fclose(file);

        #undef POP
    }



    EZ_window("DEMO", 600, 600, bitmap);

    // EZ_draw2D_clear(bitmap, EZ_BLUE);
    // EZ_draw2D_clear(bitmap, EZ_BLUE);
    // EZ_draw2D_line(bitmap, EZ_WHITE, 10, 15, 40, 32);

}

void kill(void* param) {

  EZ_freeImage(bitmap);
  
}


void draw(void* param) {}
void key(void* param) {EZ_stop();}
void keyRelease(void* param) {}
void mouse(void* param) {}




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
