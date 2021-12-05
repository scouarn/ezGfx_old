#include "ezGfx_font.h"
#include "ezGfx_utils.h"

#include <stdio.h>
#include <stdlib.h>



EZ_Font_t* EZ_createFont(int w, int h) {
	int i;

	EZ_Font_t* font = malloc( sizeof(EZ_Font_t) );
	font->h = h; 
	font->w = w;
	font->wb = w/9 + 1; /* 1 byte for 8 pixels, at least one byte */

	/* allocate for 256 pointer */
	font->data = malloc( 256 * sizeof(uint8_t*) );

	/* allocate for 256 glyphs */
	font->data[0] = calloc( 256 * font->wb * h, 1);

	/* point each pointer to the desired location */
	for (i = 1; i < 256; i++) 
		font->data[i] = font->data[i-1] + font->wb*h;


	return font;
}



void EZ_freeFont(EZ_Font_t* font) {
	free(font->data[0]);
	free(font->data);
	free(font);
}




void EZ_font_printChar(EZ_Image_t* target, unsigned int c, EZ_Font_t* font, EZ_Px_t fg, EZ_Px_t bg, int x0, int y0) {
	int x, y, xByte;

	/* loop line by line */
	for (y = 0; y < font->h; y++)
		for (xByte = 0; xByte < font->wb; xByte++)
		for (x = 0; x < 8 && x < font->w; x++) {
			EZ_Px_t* dest = &(target->px[x0 + x + 8*xByte + (y0 + y)*target->w]);

			*dest = EZ_blend(*dest, (font->data[c][xByte + y*font->wb] << x) & 0x80 ? fg : bg, ALPHA_FAST);
		}

}


const char* EZ_font_printStr(EZ_Image_t* target, const char* str, EZ_Font_t* font, EZ_Px_t fg, EZ_Px_t bg, int x0, int y0, int w_chars, int h_chars) {

	/* init cursor position */
	int x = 0, y = 0;

	/* current char to draw */
	/* forced unsigned to access "extended" ascii */
	unsigned char* c;

	/* while end of string not found and cursor still in vertical area */
	for (c = (unsigned char*)str; *c != '\0' && y < h_chars; c++) {

		/* handle control characters */
		switch (*c) {
		case '\t' :
			x += 4;
		break;

		case '\v' :
			y += 2;
		break;

		case '\a' :
			putchar('\a');
		break;

		case '\f' : /* new page */

		break;

		case '\n' :
			x = 0; y++;
		break;

		case '\r' :
			x = 0;
		break;

		case '\x7f' : /* delete */
			x--;
			EZ_font_printChar(target, ' ', font, fg, bg, x0 + x*font->w, y0 + y*font->h);
		break;

		default :
			EZ_font_printChar(target, *c, font, fg, bg, x0 + x*font->w, y0 + y*font->h);
			if (++x >= w_chars) {x = 0; y++;} /* wrapping */
		break;
		}

	}

	return (const char*)c;

}





/* https://wiki.osdev.org/PC_Screen_Font */

#define PSF_FONT_MAGIC 0x864ab572

struct __attribute__((__packed__)) PSF_font_header {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};


EZ_Font_t* EZ_load_PSF2(const char* fname) {

	/* open file */
	FILE *file = fopen(fname,"rb");

	if (file == NULL) {
		EZ_throw("Couldn't load file", fname);
		return NULL;
	}


	/* read header */
	struct PSF_font_header header;
	fread(&header, sizeof(struct PSF_font_header), 1, file);

	if (header.magic != PSF_FONT_MAGIC) {
		EZ_throw("Wrong magic number", fname);
		fclose(file);

		return NULL;
	}

	/* read font */
	EZ_Font_t* font = EZ_createFont(header.width, header.height);
	fread(font->data[0], header.bytesperglyph, 256, file);


	fclose(file);
	return font;

}



void EZ_save_PSF2(EZ_Font_t* font, const char* fname) {

	/* open file */
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		EZ_throw("Couldn't save file", fname);
		return;
	}


	/* init header */
	struct PSF_font_header header;
	header.magic = PSF_FONT_MAGIC;
	header.headersize = 32;
	header.numglyph = 256;
	header.bytesperglyph = font->h * font->wb;
	header.width  = font->w;
	header.height = font->h;

	/* write */
	fwrite(&header, sizeof(struct PSF_font_header), 1, file);
	fwrite(font->data[0], header.bytesperglyph, 256, file);

	fclose(file);

}


