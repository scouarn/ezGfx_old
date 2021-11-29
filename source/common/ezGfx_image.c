#include "ezGfx_image.h"
#include "ezGfx_utils.h"
#include <stdlib.h>
#include <stdio.h>

EZ_Image_t* EZ_createImage(int w, int h) {

	EZ_Image_t* img = malloc( sizeof(EZ_Image_t) );
	
	img->w = w;
	img->h = h;
	img->px = calloc(w*h, sizeof(EZ_Px_t));

	return img;
}


void EZ_freeImage(EZ_Image_t* img) {
	free(img->px);
	free(img);
}





EZ_Image_t* EZ_load_BMP(const char* fname) {

	EZ_Image_t* bitmap = malloc( sizeof(EZ_Image_t) );

	/* open file */
	FILE *file = fopen(fname,"rb");
	if (file == NULL) {
		EZ_throw("Couldn't load file", fname);
		bitmap->px = NULL;
		return bitmap;
	}


	/* skip n bytes */
	#define POP(n) for (int i = 0; i < n; i++) getc(file);


	uint32_t fsize, offset;

	/* read file header (fixed 14 bytes) */
	POP(2);
	fread(&fsize, 4, 1, file);
	POP(4);
	fread(&offset, 4, 1, file);


	
	/* read DIB header */
	uint32_t header_size, image_size;
	uint16_t depth;


	fread( &header_size, 4, 1, file);
	fread( &(bitmap->w), 4, 1, file);
	fread( &(bitmap->h), 4, 1, file);
	POP(2); /* planes */
	fread(&depth, 2, 1, file);
	int bytes_ppx = depth / 8;

	POP(4); /* compression */
	fread(&image_size, 4, 1, file);
	POP(16); /* resolution and color map info */


	uint32_t r_mask, g_mask, b_mask, a_mask;

	fread(&r_mask, 4, 1, file);
	fread(&g_mask, 4, 1, file);
	fread(&b_mask, 4, 1, file);
	fread(&a_mask, 4, 1, file);

	if (depth != 24 && depth != 32) EZ_error("Unsupported pixel format");
	if (a_mask == 0) EZ_throw("No alpha channel", fname);

	
	/* read pixels */

	bitmap->px = calloc( bitmap->w * bitmap->h, sizeof(EZ_Px_t) );
	fseek(file, offset, SEEK_SET); /* offset from begining */

	int bytes_per_line = image_size / bitmap->h;

	uint8_t* line_buffer = malloc( bytes_per_line );

	int x, y, c;
	for (y = bitmap->h-1; y >= 0; y--) { /* y axis is reversed in bitmap format (bottom to top) */
		fread(line_buffer, 1, bytes_per_line, file);

		for (x = 0; x < bitmap->w; x++) {
			EZ_Px_t* px = &(bitmap->px[x + y*bitmap->w]);

			/* test each color byte against each byte of each bitmask */
			for (c = 0; c < bytes_ppx; c++) {
				px->r |= line_buffer[x*bytes_ppx + c] & (r_mask >> 8*c);
				px->g |= line_buffer[x*bytes_ppx + c] & (g_mask >> 8*c);
				px->b |= line_buffer[x*bytes_ppx + c] & (b_mask >> 8*c);
				px->a |= line_buffer[x*bytes_ppx + c] & (a_mask >> 8*c);
			}

			/* set max transparency if it's not defined in the image */
			if (a_mask == 0) px->a = 255;

			}
	}

	fclose(file);
	free(line_buffer);

	return bitmap;


}

void EZ_save_BMP(EZ_Image_t* img, const char* fname) {


	/* open file */
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		EZ_throw("Couldn't save file", fname);
		return;
	}


	/* write file header */
	const uint32_t zero = 0x0, one  = 0x1;
	const uint32_t img_size = img->w * img->h * 4;
	const uint32_t offset = 138;
	const uint32_t f_size = offset + img_size;

	fputs("BM", file);           /* signature */
	fwrite(&f_size, 4, 1, file); /* file size */
	fwrite(&zero, 4, 1, file);   /* reserved */
	fwrite(&offset, 4, 1, file); /* offset */



	/* write DIB header */

	const uint32_t h_size = 124;
	const uint16_t planes = 1;
	const uint16_t depth = 32;
	const uint32_t comp = 3;
	const uint32_t intent = 2;

	const uint32_t r_mask = 0xff000000; /* unusual masking ? */
	const uint32_t g_mask = 0x00ff0000;
	const uint32_t b_mask = 0x0000ff00;
	const uint32_t a_mask = 0x000000ff;

	fwrite(&h_size,   4, 1, file); /* header size */
	fwrite(&img->w,    4, 1, file); /* width */
	fwrite(&img->h,    4, 1, file); /* height */
	fwrite(&planes,   2, 1, file); /* planes */
	fwrite(&depth,    2, 1, file); /* depth */
	fwrite(&comp,     4, 1, file); /* compression */
	fwrite(&img_size, 4, 1, file); /* img size in bytes */
	fwrite(&one,      4, 1, file); /* res */
	fwrite(&one,      4, 1, file); /* res */
	fwrite(&zero,     4, 1, file); /* palete */
	fwrite(&zero,     4, 1, file); /* palete */
	fwrite(&r_mask,   4, 1, file); /* colormask */
	fwrite(&g_mask,   4, 1, file); /* colormask */
	fwrite(&b_mask,   4, 1, file); /* colormask */
	fwrite(&a_mask,   4, 1, file); /* colormask */

	fputs("BGRs", file); /* color space type */

	int i;

	for (i = 0; i < 36; i++) /* color space endpoints (36 zero bytes) */
		fputc('\0', file); 

	for (i = 0; i < 3; i++) /* gamma (int 0 for rgb) */
		fwrite(&zero, 4, 1,  file); 

	fwrite(&intent, 4, 1, file); /* intent */

	for (i = 0; i < 3; i++) /* ICC */
		fwrite(&zero, 4, 1,  file); 


	/* px data */
	/* line per line */
	/* no padding, RGBA already 32bits 4bytes-aligned */

	for (i = img->h - 1; i >= 0; i--) //y axis mirroring
		fwrite(img->px + i*img->w, sizeof(EZ_Px_t), img->w,  file); 


	//close
	fclose(file);


}

