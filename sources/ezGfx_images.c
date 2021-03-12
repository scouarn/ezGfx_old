#include "ezGfx_images.h"


EZ_Image* EZ_images_load(const char* fname) {

    //open file
    FILE *file = fopen(fname,"rb");

    //get the size
    EZ_Image header;
    fread(&header, sizeof(EZ_Image), 1, file);

    //get the pixels
    EZ_Image* image = EZ_createImage(header.w, header.h);
    fread(image->px, sizeof(EZ_px), header.w*header.h, file);

    //return image pointer
    fclose(file);
    return image;

}




void EZ_images_save(EZ_Image* image, const char* fname) {

  FILE *file = fopen(fname,"wb");

  fwrite(image, sizeof(EZ_Image), 1, file);
  fwrite(image->px, sizeof(EZ_px), image->w*image->h, file);

  fclose(file);


}
