#include "ezGfx_fonts.h"



EZ_Font EZ_fonts_make(int w, int h) {

	EZ_Font font = {0};
	font.h_px = h; font.w_px = w;
	font.w_bytes = w/9 + 1;

	font.data = malloc(256*sizeof(char*));
	font.data[0] = calloc(256*font.w_bytes*h, 1);

	for (int i = 1; i < 256; i++) font.data[i] = font.data[i-1] + font.w_bytes*h;

	return font;
}



EZ_Font EZ_fonts_load(const char* fname) {

	FILE *file = fopen(fname,"rb");
	EZ_Font font;

	if (!file) {
		font.data = NULL;
		return font;
	}

	int h, w, wb;
	fread(&h,  sizeof(int), 1, file);
	fread(&w,  sizeof(int), 1, file);
	fread(&wb, sizeof(int), 1, file);

	font = EZ_fonts_make(w, h);
	fread(font.data[0], wb*h, 256, file);



	fclose(file);
	return font;
}



void EZ_fonts_save(EZ_Font font, const char* fname) {

	FILE *file = fopen(fname,"wb");

	fwrite(&font.h_px,    sizeof(int), 1, file);
	fwrite(&font.w_px,    sizeof(int), 1, file);
	fwrite(&font.w_bytes, sizeof(int), 1, file);
	fwrite(font.data[0], font.w_bytes*font.h_px, 256, file);

	fclose(file);
}



void EZ_fonts_free(EZ_Font font) {
	free(font.data[0]);
	free(font.data);
}


void EZ_fonts_printChar(EZ_Image target, unsigned int c, EZ_Font font, EZ_px fg, EZ_px bg, int x0, int y0) {


	for (int y = 0; y < font.h_px; y++)
		for (int xByte = 0; xByte < font.w_bytes; xByte++)
			for (int x = 0; x < 8; x++) {
				EZ_px* dest = &target.px[x0 + x + 8*xByte + (y0 + y)*target.w];

				*dest = EZ_blend(
					(font.data[c][xByte + y*font.w_bytes] << x) & 0x80 ? fg : bg, 
					*dest, ALPHA_FAST);
			}

					
}


const char* EZ_fonts_printStr(EZ_Image target, const char* str, EZ_Font font, EZ_px fg, EZ_px bg, int x0, int y0, int w_chars, int h_chars) {


	int x = 0, y = 0;
	const unsigned char* c;

	for (c = str; *c != '\0' && y < h_chars; c++) {

		switch (*c) {
			 case '\t' : //tab
				  x += 4;
				  break;

			 case '\v' : //vertical tab
				  y += 2;
				  break;

			 case '\a' : //bell
				  putchar('\a');
				  break;

			 case '\f' : //new page
				  break;

			 case '\n' : //line feed
				  x = 0; y++;
				  break;

			 case '\r' : //carriage return
				  x = 0;
				  break;

			 case '\x7f' : //delete
				  x--;
				  EZ_fonts_printChar(target, ' ', font, fg, bg, x0 + x*font.w_px, y0 + y*font.h_px);
				  break;

			 default :
				  EZ_fonts_printChar(target, *c, font, fg, bg, x0 + x*font.w_px, y0 + y*font.h_px);
				  if (++x >= w_chars) {x = 0; y++;} //wrapping
				  break;
		}

	}

	return c;

}