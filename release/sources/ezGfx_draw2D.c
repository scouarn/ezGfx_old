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




EZ_Image EZ_draw2D_loadBMP(const char* fname) {

	EZ_Image bitmap;

	//open file
	FILE *file = fopen(fname,"rb");

	//check if opened
	if (file == NULL) {
			printf("Couldn't load file.");
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

	return bitmap;


}

void EZ_draw2D_saveBMP(EZ_Image img, const char* fname) {

	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		printf("Can't save file %s\n", fname);
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

void EZ_draw2D_pixel(EZ_Image target, EZ_px col, int x1, int y1) {

	if (x1 < 0 || x1 >= target.w || y1 < 0 || y1 >= target.h)
		return;

	EZ_px* px = &(target.px[x1 + y1*target.w]);
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

static void _line(EZ_Image target, EZ_px col, int _x1, int _y1, int _x2, int _y2) {

	float slope = (float)(_x2-_x1)/(_y2-_y1);

	//clever trick to not end up with a dotted line
	if (slope > 1.0f || slope < -1.0f) {
		slope = 1.0f / slope;
		float y = (_x1 < _x2) ? _y1 : _y2;
		int x1  = MIN(_x1, _x2);
		int x2  = MAX(_x1, _x2);

		for (int x = x1; x <= x2; x++) {
			EZ_draw2D_pixel(target, col, x, y);
			y += slope;
		}

	}
	else {
		float x = (_y1 < _y2) ? _x1 : _x2;
		int y1  = MIN(_y1, _y2);
		int y2  = MAX(_y1, _y2);

		for (int y = y1; y <= y2; y++) {
			EZ_draw2D_pixel(target, col, x, y);
			x += slope;
		}

	}



}

void EZ_draw2D_line(EZ_Image target, EZ_px col, int _x1, int _y1, int _x2, int _y2) {

	//point
	if (_x1 == _x2 && _y1 == _y2) {
		EZ_draw2D_pixel(target, col, _x1, _y1);
	}

	//vertical line
	else if (_x1 == _x2) {
		int y1  = MIN(_y1, _y2);
		int y2  = MAX(_y1, _y2);
		_VLine(target, col, _x1, y1, y2);
	}

	//horizontal line
	else if (_y1 == _y2) {
		int x1  = MIN(_x1, _x2);
		int x2  = MAX(_x1, _x2);
		_HLine(target, col, _y1, x1, x2);

	}

	//any line
	else {
		_line(target, col, _x1, _y1, _x2, _y2);
	}


}

void EZ_draw2D_rect(EZ_Image target, EZ_px col, int x1, int y1, int w,  int h) {

	if (x1 >= target.w || y1 >= target.h || x1+w < 0 || y1+h < 0) return;

	//Hline and Vline already implement clipping
	_HLine(target, col, y1    , x1  , x1+w-1); // top
	_VLine(target, col, x1    , y1+1, y1+h-2); // left
	_HLine(target, col, y1+h-1, x1  , x1+w-1); // bottom
	_VLine(target, col, x1+w-1, y1+1, y1+h-2); //right

}

void EZ_draw2D_fillRect(EZ_Image target, EZ_px col, int x0, int y0, int w,  int h) {

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

void _botFlatTri(EZ_Image target, EZ_px col, int x1, int x2, int xTop, int yTop, int yBase) {

	float a = MIN(x1, x2);
	float b = MAX(x1, x2);

	float LSlope = (float)(xTop-a)/(yBase-yTop);
	float RSlope = (float)(xTop-b)/(yBase-yTop);

	for (int y = yBase; y >= yTop; y--) {
		_HLine(target, col, y, a, b);

		a += LSlope;
		b += RSlope;
	}

}


void _topFlatTri(EZ_Image target, EZ_px col, int x1, int x2, int xTop, int yTop, int yBase) {

	float a = MIN(x1, x2);
	float b = MAX(x1, x2);

	float LSlope = (float)(xTop-a)/(yBase-yTop);
	float RSlope = (float)(xTop-b)/(yBase-yTop);

	for (int y = yBase; y <= yTop; y++) {
		_HLine(target, col, y, a, b);

		a -= LSlope;
		b -= RSlope;
	}

}

void EZ_draw2D_fillTri(EZ_Image target, EZ_px col, int u1, int v1, int u2, int v2, int u3, int v3) {

	//sort points (y1 : top, y2 : mid, y3 : bot)
	int y1 = v1; int y2 = v2; int y3 = v3;
	int x1 = u1; int x2 = u2; int x3 = u3;

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
		_topFlatTri(target, col, x1, x2, x3, y3, y1);

	else if (y2 == y3)
		_botFlatTri(target, col, x2, x3, x1, y1, y3);

	else
	{

		float ratio  = (float)(y1 - y2 - 0.5f) / (y1 - y3);
		int xSlice = ratio*x3 + (1.0f - ratio)*x1;

		_botFlatTri(target, col, x2, xSlice, x1, y1, y2 - 1);
		_topFlatTri(target, col, x2, xSlice, x3, y3, y2);

	}


}

void EZ_draw2D_texTri  (EZ_Image target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3,
												EZ_Image texture,           int u1, int v1, int u2, int v2, int u3, int v3) {

													//TODO
}



void EZ_draw2D_elli(EZ_Image target, EZ_px col, int x1, int y1, int a,  int b) {


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


		EZ_draw2D_pixel(target, col, x1 + x, y1 + y);
		EZ_draw2D_pixel(target, col, x1 + x, y1 - y);
		EZ_draw2D_pixel(target, col, x1 - x, y1 + y);
		EZ_draw2D_pixel(target, col, x1 - x, y1 - y);

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


		EZ_draw2D_pixel(target, col, y1 + y, x1 + x);
		EZ_draw2D_pixel(target, col, y1 - y, x1 + x);
		EZ_draw2D_pixel(target, col, y1 + y, x1 - x);
		EZ_draw2D_pixel(target, col, y1 - y, x1 - x);

		dx += 2.0f * b2;
		x++;
	}




}

void EZ_draw2D_fillElli(EZ_Image target, EZ_px col, int x1, int y1, int a,  int b) {

	//same but with Hlines

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


		_HLine(target, col, y1 + y, x1 - x, x1 + x);
		_HLine(target, col, y1 - y, x1 - x, x1 + x);

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


		_HLine(target, col, y1 + x, x1 - y, x1 + y);
		_HLine(target, col, y1 - x, x1 - y, x1 + y);

		dx += 2.0f * b2;
		x++;
	}

}



void EZ_draw2D_image(EZ_Image target, EZ_Image source, int x0, int y0) {

	if (x0 >= target.w || y0 >= target.h || x0+source.w < 0 || y0+source.h < 0) return;

	int tx1 = MAX(0, x0);
	int tx2 = MAX(0, x0+source.w);

	int ty1 = MAX(0, y0);
	int ty2 = MAX(0, y0+source.h);


	for (int tx = tx1; tx < tx2; tx++) {
		for (int ty = ty1; ty < ty2; ty++)
		{
			int sx = tx - tx1;
			int sy = ty - ty1;

			EZ_px  col = source.px[sx + sy*source.w];
			EZ_px* px  = &(target.px[tx + ty*target.w]);

			_blend(px, col);
		}

	}


}


void EZ_draw2D_croppedImage(EZ_Image target, EZ_Image source, int x0, int y0, int u0, int v0, int w, int h) {

	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	int tx1 = MAX(0, x0);
	int tx2 = MAX(0, x0+w);

	int ty1 = MAX(0, y0);
	int ty2 = MAX(0, y0+h);


	for (int tx = tx1; tx < tx2; tx++) {
		for (int ty = ty1; ty < ty2; ty++)
		{
			int sx = (tx - tx1 + u0);
			int sy = (ty - ty1 + v0);

			EZ_px  col = source.px[sx + sy*source.w];
			EZ_px* px  = &(target.px[tx + ty*target.w]);

			_blend(px, col);
		}

	}

}

void EZ_draw2D_resizedImage(EZ_Image target, EZ_Image source, int x0, int y0, int w, int h) {

	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	int tx1 = MAX(0, x0);
	int tx2 = MAX(0, x0+w);

	int ty1 = MAX(0, y0);
	int ty2 = MAX(0, y0+h);

	float xRatio = (float)source.w / w;
	float yRatio = (float)source.h / h;

	for (int tx = tx1; tx < tx2; tx++) {
		for (int ty = ty1; ty < ty2; ty++)
		{
			int sx = (tx - tx1)*xRatio;
			int sy = (ty - ty1)*yRatio;

			EZ_px  col = source.px[sx + sy*source.w];
			EZ_px* px  = &(target.px[tx + ty*target.w]);

			_blend(px, col);
		}

	}

}
