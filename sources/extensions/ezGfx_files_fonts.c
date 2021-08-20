#include "ezGfx.h"

#include <stdio.h>
#include <stdlib.h>



//https://wiki.osdev.org/PC_Screen_Font

#define PSF_FONT_MAGIC 0x864ab572
 
struct PSF_font_header {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};


EZ_Font EZ_load_PSF2(const char* fname) {

	//open file
	FILE *file = fopen(fname,"rb");
	EZ_Font font;

	if (file == NULL) {
		WARNING("Couldn't load file %s\n", fname);
		return font;
	}


	//read header
	struct PSF_font_header header;
	fread(&header, sizeof(struct PSF_font_header), 1, file);

	if (header.magic != PSF_FONT_MAGIC) {
		WARNING("Wrong magic number in %s", fname);
		fclose(file);
		return font;
	}


	//setup font
	font = EZ_createFont(header.width, header.height);


	if (font.data[0] == NULL) {
		WARNING("Couldn't allocate memory for font %s", fname);
		fclose(file);
		return font;
	}

	//read font data
	fread(font.data[0], header.bytesperglyph, 256, file);

	//close
	fclose(file);
	return font;
}



void EZ_save_PSF2(const EZ_Font font, const char* fname) {

	//open file
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}


	//init header
	struct PSF_font_header header;
	header.magic = PSF_FONT_MAGIC;
	header.headersize = 32;
	header.numglyph = 256;
	header.bytesperglyph = font.h_px * font.w_bytes;
	header.height = font.h_px;
	header.width  = font.w_px;

	//write header
	fwrite(&header, sizeof(struct PSF_font_header), 1, file);


	//write font data
	int written = fwrite(font.data[0], header.bytesperglyph, 256, file);

	//error check
	ASSERTW(written > 0, "Nothing was written to %s", fname);

	//close
	fclose(file);

}


