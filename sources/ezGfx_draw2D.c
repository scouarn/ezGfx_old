#include "ezGfx_draw2D.h"


/*
TODO :

"any line" clipping -> kinda easy
triangle clipping  -> see olc

textriangle -> research

transformed image -> redo the matrix thing

sampling on stuff...

*/


static enum EZ_blendMode alphaBlending = ALPHA_BLEND;
static enum interMode interpolation = INTER_NEAREST;

static void _blend(EZ_px*, EZ_px);

static int TX, TY;

void EZ_draw2D_translate(int x, int y) 	    {TX += x; TY += y;}
void EZ_draw2D_setTranslate(int x, int y)   {TX =  x; TY =  y;}
void EZ_draw2D_getTranslate(int* x, int* y) {*x = TX; *y = TY;}


EZ_Image EZ_draw2D_loadBMP(const char* fname) {

	EZ_Image bitmap;

	//open file
	FILE *file = fopen(fname,"rb");

	//check if opened
	if (file == NULL) {
		WARNING("Couldn't load file %s", fname);
		bitmap.px = NULL;
		return bitmap;
	}


	char buffer[64]; //for garbage
	#define POP(n) fread(&buffer, 1, n, file)

	uint32_t fsize, offset;

	//read file header (fixed 14 bytes)
	POP(2);
	fread(&fsize, 4, 1, file);
	POP(4);
	fread(&offset, 4, 1, file);


	
	//read DIB header
	uint32_t header_size, image_size;
	uint16_t depth;


	fread(&header_size, 4, 1, file);
	fread(&bitmap.w, 4, 1, file);
	fread(&bitmap.h, 4, 1, file);
	POP(2); //planes
	fread(&depth, 2, 1, file);
	int bytes_ppx = depth / 8;

	POP(4); //compression
	fread(&image_size, 4, 1, file);
	POP(16); //resolution and color map info


	uint32_t r_mask, g_mask, b_mask, a_mask;

	fread(&r_mask, 4, 1, file);
	fread(&g_mask, 4, 1, file);
	fread(&b_mask, 4, 1, file);
	fread(&a_mask, 4, 1, file);

	if (depth != 24 && depth != 32) ERROR("Unsupported %dbits pixel format", depth);
	if (a_mask == 0) WARNING("No alpha channel in file %s", fname);

	
	//read pixels

	bitmap.px = calloc(bitmap.w*bitmap.h, sizeof(EZ_px));

	if (bitmap.px == NULL) {
		WARNING("Couldn't allocate memory for image %s\n", fname);
		return bitmap;
	}


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

			if (a_mask == 0) px->col.a = 255;

			}
	}


	//close
	fclose(file);

	#undef POP

	return bitmap;


}

void EZ_draw2D_saveBMP(EZ_Image img, const char* fname) {

	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}

	const uint32_t zero = 0x0, one  = 0x1;

	//file header

	const uint32_t img_size = img.w*img.h * 4;
	const uint32_t offset = 138;
	const uint32_t f_size = offset + img_size;

	fputs("BM", file); //signature
	fwrite(&f_size, 4, 1, file); //file size
	fwrite(&zero, 4, 1, file); //reserved
	fwrite(&offset, 4, 1, file); //offset



	//DIB header

	const uint32_t h_size = 124;
	const uint16_t planes = 1;
	const uint16_t depth = 32;
	const uint32_t comp = 3;
	const uint32_t intent = 2;

	const uint32_t r_mask = 0xff000000; //unusual masking ?
	const uint32_t g_mask = 0x00ff0000;
	const uint32_t b_mask = 0x0000ff00;
	const uint32_t a_mask = 0x000000ff;

	fwrite(&h_size,   4, 1, file); //header size
	fwrite(&img.w,    4, 1, file); //width
	fwrite(&img.h,    4, 1, file); //height
	fwrite(&planes,   2, 1, file); //planes
	fwrite(&depth,    2, 1, file); //depth
	fwrite(&comp,     4, 1, file); //compression
	fwrite(&img_size, 4, 1, file); //img size in bytes
	fwrite(&one,      4, 1, file); //res
	fwrite(&one,      4, 1, file); //res
	fwrite(&zero,     4, 1, file); //palete
	fwrite(&zero,     4, 1, file); //palete
	fwrite(&r_mask,   4, 1, file); //colormask
	fwrite(&g_mask,   4, 1, file); //colormask
	fwrite(&b_mask,   4, 1, file); //colormask
	fwrite(&a_mask,   4, 1, file); //colormask

	fputs("BGRs", file); //color space type

	for (int i = 0; i < 36; i++) //color space endpoints
		fputc('\0', file); 

	for (int i = 0; i < 3; i++) //gamma
		fwrite(&zero, 4, 1,  file); 

	fwrite(&intent, 4, 1, file); //intent

	for (int i = 0; i < 3; i++) //ICC
		fwrite(&zero, 4, 1,  file); 


	//px data
	
	//line per line
	//no padding, RGBA already 4bytes-aligned

	for (int y = img.h-1; y >= 0; y--) //y axis mirroring
		fwrite(img.px + y*img.w, sizeof(EZ_px), img.w,  file); 


	fclose(file);


}

EZ_Font EZ_draw2D_makeFont(int w, int h) {

	EZ_Font font = {0};
	font.h_px = h; font.w_px = w;
	font.w_bytes = w/9 + 1;

	font.data = malloc(256*sizeof(char*));
	font.data[0] = calloc(256*font.w_bytes*h, 1);

	if (font.data[0] == NULL) {
		WARNING("Couldn't allocate memory for font\n");
		font.data = NULL;
		return font;
	}

	for (int i = 1; i < 256; i++) font.data[i] = font.data[i-1] + font.w_bytes*h;

	return font;
}



EZ_Font EZ_draw2D_loadFont(const char* fname) {

	FILE *file = fopen(fname,"rb");
	EZ_Font font;

	if (file == NULL) {
		WARNING("Couldn't load file %s\n", fname);
		font.data = NULL;
		return font;
	}


	int h, w, wb;
	fread(&h,  sizeof(int), 1, file);
	fread(&w,  sizeof(int), 1, file);
	fread(&wb, sizeof(int), 1, file);

	font = EZ_draw2D_makeFont(w, h);
	fread(font.data[0], wb*h, 256, file);



	fclose(file);
	return font;
}



void EZ_draw2D_saveFont(EZ_Font font, const char* fname) {

	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}

	fwrite(&font.h_px,    sizeof(int), 1, file);
	fwrite(&font.w_px,    sizeof(int), 1, file);
	fwrite(&font.w_bytes, sizeof(int), 1, file);
	fwrite(font.data[0], font.w_bytes*font.h_px, 256, file);

	fclose(file);
}



void EZ_draw2D_freeFont(EZ_Font font) {
	free(font.data[0]);
	free(font.data);
}


void EZ_draw2D_printChar(EZ_Image target, unsigned int c, EZ_Font font, EZ_px fg, EZ_px bg, int x0, int y0) {

	x0 += TX; y0 += TY;

	for (int y = 0; y < font.h_px; y++)
		for (int xByte = 0; xByte < font.w_bytes; xByte++)
			for (int x = 0; x < 8; x++) {
				EZ_px* dest = &target.px[x0 + x + 8*xByte + (y0 + y)*target.w];

				_blend(dest, (font.data[c][xByte + y*font.w_bytes] << x) & 0x80 ? fg : bg);
			}

					
}


const char* EZ_draw2D_printStr(EZ_Image target, const char* str, EZ_Font font, EZ_px fg, EZ_px bg, int x0, int y0, int w_chars, int h_chars) {

	int x = 0, y = 0;
	const unsigned char* c;

	for (c = (const unsigned char*)str; *c != '\0' && y < h_chars; c++) {

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
				  EZ_draw2D_printChar(target, ' ', font, fg, bg, x0 + x*font.w_px, y0 + y*font.h_px);
				  break;

			 default :
				  EZ_draw2D_printChar(target, *c, font, fg, bg, x0 + x*font.w_px, y0 + y*font.h_px);
				  if (++x >= w_chars) {x = 0; y++;} //wrapping
				  break;
		}

	}

	return (const char*)c;

}


void EZ_draw2D_alphaMode(enum EZ_blendMode mode) {
	alphaBlending = mode;
}

void EZ_draw2D_interMode(enum interMode mode) {
	interpolation = mode;
}

void EZ_draw2D_clear(EZ_Image target, EZ_px col) {
	for (int i = 0; i < target.h * target.w; i++)
		target.px[i] = col;
}

static void _blend(EZ_px* px, EZ_px col) {
	*px = EZ_blend(col, *px, alphaBlending);
}

void EZ_draw2D_pixel(EZ_Image target, EZ_px col, int x0, int y0) {
	x0 += TX; y0 += TY;

	if (x0 < 0 || x0 >= target.w || y0 < 0 || y0 >= target.h)
		return;

	EZ_px* px = &(target.px[x0 + y0*target.w]);
	_blend(px, col);
}


static void _VLine(EZ_Image target, EZ_px col, int x, int _y1, int _y2) {
	
	if (x < 0 || x >= target.w) return;

	int y1 = MAX(0, _y1);
	int y2 = MIN(target.h - 1, _y2);

	for (int h = y1*target.w; h <= y2*target.w; h+=target.w)
		_blend(&target.px[x+h], col);

}

static  void _HLine(EZ_Image target, EZ_px col, int y, int _x1, int _x2) {

	if (y < 0 || y >= target.h) return;

	int x1 = MAX(0, _x1);
	int x2 = MIN(target.w - 1, _x2);

	int h  = target.w * y;
	for (int x = x1; x <= x2; x++)
		_blend(&target.px[x+h], col);

}

static void _line(EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2) {

	float slope = (float)(y2-y1)/(x2-x1);
	float ordog = y1 - x1 * slope;


	//clever trick to not end up with a dotted line
	if (slope > 1.0f || slope < -1.0f) {

		int ty1  = CLAMP(MIN(y1, y2), 0, target.h);
		int ty2  = CLAMP(MAX(y1, y2), 0, target.h);

		float x = (ty1 - ordog)/slope;

		for (int y = ty1; y <= ty2; y++) {
			EZ_draw2D_pixel(target, col, x, y);
			x += 1.0f/slope;
		}


	}
	else {

		int tx1  = CLAMP(MIN(x1, x2), 0, target.w);
		int tx2  = CLAMP(MAX(x1, x2), 0, target.w);

		float y = tx1*slope + ordog;

		for (int x = tx1; x <= tx2; x++) {
			EZ_draw2D_pixel(target, col, x, y);
			y += slope;
		}

	}



}

void EZ_draw2D_line(EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2) {

	x1 += TX; y1 += TY; x2 += TX; y2 += TY;

	//point
	if (x1 == x2 && y1 == y2)
		EZ_draw2D_pixel(target, col, x1, y1);

	//vertical line
	else if (x1 == x2)
		_VLine(target, col, x1, MIN(y1, y2), MAX(y1, y2));

	//horizontal line
	else if (y1 == y2)
		_HLine(target, col, y1, MIN(x1, x2), MAX(x1, x2));

	//any line
	else
		_line(target, col, x1, y1, x2, y2);


}

void EZ_draw2D_rect(EZ_Image target, EZ_px col, int x0, int y0, int w,  int h) {

	x0 += TX; y0 += TY;

	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	//Hline and Vline already implement clipping
	_HLine(target, col, y0    , x0  , x0+w-1); // top
	_VLine(target, col, x0    , y0+1, y0+h-2); // left
	_HLine(target, col, y0+h-1, x0  , x0+w-1); // bottom
	_VLine(target, col, x0+w-1, y0+1, y0+h-2); //right

}

void EZ_draw2D_fillRect(EZ_Image target, EZ_px col, int x0, int y0, int w,  int h) {
	
	x0 += TX; y0 += TY;

	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	int y1 = MAX(0, y0);
	int y2 = MAX(0, y0+h);

	for (int y = y1; y < y2; y++)
		_HLine(target, col, y, x0, x0+w-1);

}


void EZ_draw2D_tri(EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3) {
	EZ_draw2D_line(target, col, x1, y1, x2, y2);
	EZ_draw2D_line(target, col, x2, y2, x3, y3);
	EZ_draw2D_line(target, col, x3, y3, x1, y1);
}


static void _flatTri(EZ_Image target, EZ_px col, int x1, int x2, int xTop, int yTop, int yBase) {
	

	float Lslope = (float)(x1-xTop)/(yBase-yTop);
	float Rslope = (float)(x2-xTop)/(yBase-yTop);

	float Lordog = xTop - yTop * Lslope;
	float Rordog = xTop - yTop * Rslope;

	int ty1 = CLAMP(MIN(yTop, yBase),  0, target.h);
	int ty2 = CLAMP(MAX(yTop, yBase),   0, target.h);

	float Lx = Lordog + ty1 * Lslope;
	float Rx = Rordog + ty1 * Rslope;

	for (int y = ty1; y <= ty2; y++) {
		_HLine(target, col, y, Lx, Rx);
		Lx += Lslope; Rx += Rslope;
	}


}

void EZ_draw2D_fillTri(EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3) {

	//sort points (y1 : top, y2 : mid, y3 : bot)
	
	x1 += TX; x2 += TX; x3 += TX;
	y1 += TY; y2 += TY; y3 += TY;

	if (y1 > y2) {
		int t;
		t = y1; y1 = y2; y2 = t;
		t = x1; x1 = x2; x2 = t;
	}
	if (y1 > y3) {
		int t;
		t = y1; y1 = y3; y3 = t;
		t = x1; x1 = x3; x3 = t;
	}
	if (y2 > y3) {
		int t;
		t = y2; y2 = y3; y3 = t;
		t = x2; x2 = x3; x3 = t;
	}


	if (y1 == y2)
		_flatTri(target, col, MIN(x1, x2), MAX(x1, x2), x3, y3, y1);

	else if (y2 == y3)
		_flatTri(target, col, MIN(x2, x3), MAX(x2, x3), x1, y1, y3);

	else
	{

		float ratio  = (float)(y1 - y2 - 0.5f) / (y1 - y3);
		int xSlice = ratio*x3 + (1.0f - ratio)*x1;

		_flatTri(target, col, MIN(x2, xSlice), MAX(x2, xSlice), x1, y1, y2 - 1);
		_flatTri(target, col, MIN(x2, xSlice), MAX(x2, xSlice), x3, y3, y2);

	}


}

void EZ_draw2D_texTri  (EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3,
												EZ_Image texture,           int u1, int v1, int u2, int v2, int u3, int v3) {

													//TODO
}



void EZ_draw2D_ellipse(EZ_Image target, EZ_px col, int x0, int y0, int a,  int b) {


	//https://www.geeksforgeeks.org/midpoint-ellipse-drawing-algorithm/

	int x, y;
	float p, dx, dy, a2, b2;


	x = 0; y = b;
	a2 = (float)a*a; b2 = (float)b*b;
	dx = 0; dy = 2.0f * a2 * y;
	p  = b2 - a2*y + 0.25f*a2;

	while (dx < dy && x < target.w && y >= 0) {


		if (p <= 0) {
			p  += dx + b2;
		}
		else {
			p  += dx - dy + b2;
			dy -= 2.0f * a2;
			y--;
		}


		EZ_draw2D_pixel(target, col, x0 + x, y0 + y);
		EZ_draw2D_pixel(target, col, x0 + x, y0 - y);
		EZ_draw2D_pixel(target, col, x0 - x, y0 + y);
		EZ_draw2D_pixel(target, col, x0 - x, y0 - y);

		dx += 2.0f * b2;
		x++;
	}


	x = 0; y = a;
	b2 = (float)a*a; a2 = (float)b*b;

	dx = 0; dy = 2.0f * a2 * y;
	p  = b2 - a2*y + 0.25f*a2;

	while (dx < dy && x < target.w && y >= 0) {

		if (p <= 0) {
			p  += dx + b2;
		}
		else
		{
				p  += dx - dy + b2;
				dy -= 2.0f * a2;
				y--;
		}


		EZ_draw2D_pixel(target, col, x0 + y, y0 + x);
		EZ_draw2D_pixel(target, col, x0 - y, y0 + x);
		EZ_draw2D_pixel(target, col, x0 + y, y0 - x);
		EZ_draw2D_pixel(target, col, x0 - y, y0 - x);

		dx += 2.0f * b2;
		x++;
	}




}

void EZ_draw2D_fillEllipse(EZ_Image target, EZ_px col, int x0, int y0, int a,  int b) {

	//same but with Hlines


	x0 += TX; y0 += TY;

	int x, y;
	float p, dx, dy, a2, b2;


	x = 0; y = b;
	a2 = (float)a*a; b2 = (float)b*b;
	dx = 0; dy = 2.0f * a2 * y;
	p  = b2 - a2*y + 0.25f*a2;

	while (dx < dy && x < target.w && y >= 0) {


		if (p <= 0) {
			p  += dx + b2;
		}
		else {
			p  += dx - dy + b2;
			dy -= 2.0f * a2;
			y--;
		}


		_HLine(target, col, y0 + y, x0 - x, x0 + x);
		_HLine(target, col, y0 - y, x0 - x, x0 + x);

		dx += 2.0f * b2;
		x++;
	}


	x = 0; y = a;
	b2 = (float)a*a; a2 = (float)b*b;

	dx = 0; dy = 2.0f * a2 * y;
	p  = b2 - a2*y + 0.25f*a2;

	while (dx < dy && x < target.w && y >= 0) {

		if (p <= 0 && x < target.w && y >= 0) {
			p  += dx + b2;
		}
		else
		{
				p  += dx - dy + b2;
				dy -= 2.0f * a2;
				y--;
		}


		_HLine(target, col, y0 + x, x0 - y, x0 + y);
		_HLine(target, col, y0 - x, x0 - y, x0 + y);

		dx += 2.0f * b2;
		x++;
	}

}



void EZ_draw2D_image(EZ_Image target, EZ_Image source, int x0, int y0) {

	x0 += TX; y0 += TY;

	if (x0 >= target.w || y0 >= target.h || x0+source.w < 0 || y0+source.h < 0) return;

	int tx1 = MAX(0, x0);
	int tx2 = CLAMP(x0+source.w, 0, target.w);

	int ty1 = MAX(0, y0);
	int ty2 = CLAMP(y0+source.h, 0, target.h);


	for (int tx = tx1; tx < tx2; tx++) {
		for (int ty = ty1; ty < ty2; ty++)
		{
			int sx = tx - x0;
			int sy = ty - y0;

			EZ_px  col = source.px[sx + sy*source.w];
			EZ_px* px  = &(target.px[tx + ty*target.w]);

			_blend(px, col);
		}

	}


}


void EZ_draw2D_croppedImage(EZ_Image target, EZ_Image source, int x0, int y0, int u0, int v0, int w, int h) {

	x0 += TX; y0 += TY;

	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	int tx1 = MAX(0, x0);
	int tx2 = CLAMP(x0+w, 0, target.w);

	int ty1 = MAX(0, y0);
	int ty2 = CLAMP(y0+h, 0, target.h);


	for (int tx = tx1; tx < tx2; tx++) {
		for (int ty = ty1; ty < ty2; ty++)
		{
			int sx = (tx - x0 + u0);
			int sy = (ty - y0 + v0);

			EZ_px  col = source.px[sx + sy*source.w];
			EZ_px* px  = &(target.px[tx + ty*target.w]);

			_blend(px, col);
		}

	}

}

void EZ_draw2D_resizedImage(EZ_Image target, EZ_Image source, int x0, int y0, int w, int h) {

	x0 += TX; y0 += TY;

	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	int tx1 = MAX(0, x0);
	int tx2 = CLAMP(x0+w, 0, target.w);

	int ty1 = MAX(0, y0);
	int ty2 = CLAMP(y0+h, 0, target.h);

	float xRatio = (float)source.w / w;
	float yRatio = (float)source.h / h;

	for (int tx = tx1; tx < tx2; tx++) {
		for (int ty = ty1; ty < ty2; ty++)
		{
			int sx = (tx - x0)*xRatio;
			int sy = (ty - y0)*yRatio;

			EZ_px  col = source.px[sx + sy*source.w];
			EZ_px* px  = &(target.px[tx + ty*target.w]);

			_blend(px, col);
		}

	}

}


void EZ_draw2D_transformImage(EZ_Image target, EZ_Image source, mat3x3* transformation) {}