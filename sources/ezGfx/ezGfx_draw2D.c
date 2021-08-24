#include "ezGfx/ezGfx_draw2D.h"



static enum EZ_BlendMode alphaBlending = ALPHA_BLEND;

static void _blend(EZ_Px*, EZ_Px);

//translation vector operations
static int TX, TY;
void EZ_draw2D_translate(int x, int y) 	    {TX += x; TY += y;}
void EZ_draw2D_setTranslate(int x, int y)   {TX =  x; TY =  y;}
void EZ_draw2D_getTranslate(int* x, int* y) {*x = TX; *y = TY;}




void EZ_draw2D_printChar(EZ_Image target, unsigned int c, const EZ_Font font, EZ_Px fg, EZ_Px bg, int x0, int y0) {

	//translate
	x0 += TX; y0 += TY;

	//loop line by line
	for (int y = 0; y < font.h_px; y++)
		for (int xByte = 0; xByte < font.w_bytes; xByte++)
		for (int x = 0; x < 8; x++) {
			EZ_Px* dest = &target.px[x0 + x + 8*xByte + (y0 + y)*target.w];

			_blend(dest, (font.data[c][xByte + y*font.w_bytes] << x) & 0x80 ? fg : bg);
		}

}


const char* EZ_draw2D_printStr(EZ_Image target, const char* str, const EZ_Font font, EZ_Px fg, EZ_Px bg, int x0, int y0, int w_chars, int h_chars) {

	//init cursor position
	int x = 0, y = 0;

	//current char to draw 
	//forced unsigned to access "extended" ascii
	const unsigned char* c;

	//while end of string not found and cursor still in vertical area
	for (c = (const unsigned char*)str; *c != '\0' && y < h_chars; c++) {

		//handle control characters
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


//alpha blending
void EZ_draw2D_setBlendMode(enum EZ_BlendMode mode) {
	alphaBlending = mode;
}
//alpha blending
static void _blend(EZ_Px* px, EZ_Px col) {
	*px = EZ_blend(col, *px, alphaBlending);
}


void EZ_draw2D_clear(EZ_Image target, EZ_Px col) {
	//fill everything with one color
	for (int i = 0; i < target.h * target.w; i++)
		target.px[i] = col;
}


void EZ_draw2D_pixel(EZ_Image target, EZ_Px col, int x0, int y0) {
	//translate
	x0 += TX; y0 += TY;

	//return if ousite of drawing area
	if (x0 < 0 || x0 >= target.w || y0 < 0 || y0 >= target.h)
		return;

	//paint pixel
	EZ_Px* px = &(target.px[x0 + y0*target.w]);
	_blend(px, col);
}


static void _VLine(EZ_Image target, EZ_Px col, int x, int _y1, int _y2) {
	
	//return if ousite of drawing area
	if (x < 0 || x >= target.w) 			return;
	if (_y1 < 0 && _y2 < 0) 				return;
	if (_y1 >= target.h && _y2 >= target.h) return;

	//clip to fit target
	int y1 = CLAMP(_y1, 0, target.h-1);
	int y2 = CLAMP(_y2, 0, target.h-1);

	//draw line
	for (int h = y1*target.w; h <= y2*target.w; h+=target.w)
		_blend(&target.px[x+h], col);

}

static  void _HLine(EZ_Image target, EZ_Px col, int y, int _x1, int _x2) {

	//return if ousite of drawing area
	if (y < 0 || y >= target.h) 			return;
	if (_x1 < 0 && _x2 < 0) 				return;
	if (_x1 >= target.w && _x2 >= target.w) return;

	//clip to fit target
	int x1 = CLAMP(_x1, 0, target.w-1);
	int x2 = CLAMP(_x2, 0, target.w-1);

	//draw line
	int h  = target.w * y;
	for (int x = x1; x <= x2; x++)
		_blend(&target.px[x+h], col);

}

static void _line(EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2) {

	//dont divide by 0 !
	//x2 != x1 because _VLine would have be called instead
	//y2 != y1 because _HLine

	//y = slope * x  +  ordog
	float slope = (float)(y2-y1)/(x2-x1);
	float ordog = y1 - x1 * slope;


	//clever trick to not end up with a dotted line
	if (slope > 1.0f || slope < -1.0f) {

		//clip to fit target
		int ty1  = CLAMP(MIN(y1, y2), 0, target.h);
		int ty2  = CLAMP(MAX(y1, y2), 0, target.h);

		//get the value of x at ty1
		float x = (ty1 - ordog)/slope;

		//draw line
		for (int y = ty1; y <= ty2; y++) {
			EZ_draw2D_pixel(target, col, x, y);
			x += 1.0f/slope;
		}


	}
	else {
		
		//clip to fit target
		int tx1  = CLAMP(MIN(x1, x2), 0, target.w);
		int tx2  = CLAMP(MAX(x1, x2), 0, target.w);

		//get the value of y at tx1
		float y = tx1*slope + ordog;

		//draw line
		for (int x = tx1; x <= tx2; x++) {
			EZ_draw2D_pixel(target, col, x, y);
			y += slope;
		}

	}



}

void EZ_draw2D_line(EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2) {

	//translate
	x1 += TX; y1 += TY; 
	x2 += TX; y2 += TY;

	//draw single point
	if (x1 == x2 && y1 == y2)
		EZ_draw2D_pixel(target, col, x1, y1);

	//draw vertical line
	else if (x1 == x2)
		_VLine(target, col, x1, MIN(y1, y2), MAX(y1, y2));

	//draw horizontal line
	else if (y1 == y2)
		_HLine(target, col, y1, MIN(x1, x2), MAX(x1, x2));

	//draw any other line
	else
		_line(target, col, x1, y1, x2, y2);


}

void EZ_draw2D_rect(EZ_Image target, EZ_Px col, int x0, int y0, int w,  int h) {

	//translate
	x0 += TX; y0 += TY;

	//return if outside of drawing area
	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	//draw for edges
	//Hline and Vline already implement clipping
	_HLine(target, col, y0    , x0  , x0+w-1); // top
	_VLine(target, col, x0    , y0+1, y0+h-2); // left
	_HLine(target, col, y0+h-1, x0  , x0+w-1); // bottom
	_VLine(target, col, x0+w-1, y0+1, y0+h-2); //right

}

void EZ_draw2D_fillRect(EZ_Image target, EZ_Px col, int x0, int y0, int w,  int h) {
	
	//translate
	x0 += TX; y0 += TY;

	//return if outside of drawing area
	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	//clip to fit target
	int y1 = MAX(0, y0);
	int y2 = MAX(0, y0+h);

	//draw horizontal lines
	for (int y = y1; y < y2; y++)
		_HLine(target, col, y, x0, x0+w-1);

}


void EZ_draw2D_tri(EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2, int x3, int y3) {
	//translate
	x1 += TX; y1 += TY;
	x2 += TX; y2 += TY;
	x3 += TX; y3 += TY;

	//draw 3 edges
	//clipping implemented in these functions
	EZ_draw2D_line(target, col, x1, y1, x2, y2);
	EZ_draw2D_line(target, col, x2, y2, x3, y3);
	EZ_draw2D_line(target, col, x3, y3, x1, y1);
}


static void _flatTri(EZ_Image target, EZ_Px col, int x1, int x2, int xTop, int yTop, int yBase) {
	
	//dont divide by 0 !
	if (yBase-yTop == 0) return;

	//equation for the left and right lines
	float Lslope = (float)(x1-xTop)/(yBase-yTop);
	float Rslope = (float)(x2-xTop)/(yBase-yTop);

	float Lordog = xTop - yTop * Lslope;
	float Rordog = xTop - yTop * Rslope;

	//clip to fit drawing area
	int y1 = CLAMP(MIN(yTop, yBase),  0, target.h);
	int y2 = CLAMP(MAX(yTop, yBase),  0, target.h);

	//get values of x for left and right line at y1
	float Lx = Lordog + y1 * Lslope;
	float Rx = Rordog + y1 * Rslope;


	//draw vertical lines from left to right (between Lx and Rx)
	for (int y = y1; y <= y2; y++) {
		_HLine(target, col, y, Lx, Rx);
		Lx += Lslope; Rx += Rslope;
	}


}

void EZ_draw2D_fillTri(EZ_Image target, EZ_Px col, int x1, int y1, int x2, int y2, int x3, int y3) {

	
	//translates
	x1 += TX; y1 += TY;
	x2 += TX; y2 += TY;
	x3 += TX; y3 += TY;

	//sort points (y1 : top, y2 : mid, y3 : bot)
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

	//triangle flat on the top
	if (y1 == y2)
		_flatTri(target, col, MIN(x1, x2), MAX(x1, x2), x3, y3, y1);

	//triangle flat on the bottom
	else if (y2 == y3)
		_flatTri(target, col, MIN(x2, x3), MAX(x2, x3), x1, y1, y3);

	else {

		//slice triangle in two flat triangles
		float ratio  = (float)(y1 - y2 - 0.5f) / (y1 - y3);
		int xSlice = ratio*x3 + (1.0f - ratio)*x1;

		//draw the 2 new triangles
		_flatTri(target, col, MIN(x2, xSlice), MAX(x2, xSlice), x1, y1, y2 - 1);
		_flatTri(target, col, MIN(x2, xSlice), MAX(x2, xSlice), x3, y3, y2);

	}

}


void EZ_draw2D_ellipse(EZ_Image target, EZ_Px col, int x0, int y0, int a,  int b) {

	//weird algorithm, see :
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

void EZ_draw2D_fillEllipse(EZ_Image target, EZ_Px col, int x0, int y0, int a,  int b) {

	//same but with Hlines
	//translate because Hline doesn't translate,
	//not like EZ_draw2D_pixel
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



void EZ_draw2D_image(EZ_Image target, const EZ_Image source, int x0, int y0) {

	//translate
	x0 += TX; y0 += TY;

	//return if outside of drawing area
	if (x0 >= target.w || y0 >= target.h || x0+source.w < 0 || y0+source.h < 0) return;

	//clip to fit target
	int x1 = MAX(0, x0);
	int x2 = CLAMP(x0+source.w, 0, target.w);

	int y1 = MAX(0, y0);
	int y2 = CLAMP(y0+source.h, 0, target.h);

	//scan accross target space
	for (int x = x1; x < x2; x++) 
	for (int y = y1; y < y2; y++) {
		
		//convert to source space
		int sx = x - x0;
		int sy = y - y0;

		//paint pixel
		EZ_Px  col = source.px[sx + sy*source.w];
		EZ_Px* px  = &(target.px[x + y*target.w]);

		_blend(px, col);
	}


}


void EZ_draw2D_croppedImage(EZ_Image target, const EZ_Image source, int x0, int y0, int u0, int v0, int w, int h) {

	//translate
	x0 += TX; y0 += TY;

	//return if outside of drawing area
	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	//clip to fit target
	int x1 = MAX(0, x0);
	int x2 = CLAMP(x0+w, 0, target.w);

	int y1 = MAX(0, y0);
	int y2 = CLAMP(y0+h, 0, target.h);


	//scan accross target space
	for (int x = x1; x < x2; x++) 
	for (int y = y1; y < y2; y++) {
	
		//convert to source space
		int sx = x - x0 + u0;
		int sy = y - y0 + v0;

		//paint pixel
		EZ_Px  col = source.px[sx + sy*source.w];
		EZ_Px* px  = &(target.px[x + y*target.w]);

		_blend(px, col);

	}

}

void EZ_draw2D_resizedImage(EZ_Image target, const EZ_Image source, int x0, int y0, int w, int h) {

	//translate
	x0 += TX; y0 += TY;

	//return if outside of drawing area
	if (x0 >= target.w || y0 >= target.h || x0+w < 0 || y0+h < 0) return;

	//clip to fit target
	int x1 = MAX(0, x0);
	int x2 = CLAMP(x0+w, 0, target.w);

	int y1 = MAX(0, y0);
	int y2 = CLAMP(y0+h, 0, target.h);


	//compute x and y scaling factors
	float xRatio = (float)source.w / w;
	float yRatio = (float)source.h / h;

	//scan accross target space
	for (int x = x1; x < x2; x++)
	for (int y = y1; y < y2; y++) {
	
		//convert to source space	
		int sx = (x - x0)*xRatio;
		int sy = (y - y0)*yRatio;

		//paint pixel
		EZ_Px  col = source.px[sx + sy*source.w];
		EZ_Px* px  = &(target.px[x + y*target.w]);

		_blend(px, col);
	}

}


void EZ_draw2D_transformedImage(EZ_Image target, const EZ_Image source, const mat3x3* trans) {


	//compute target-space bounding box
	int x1, x2, y1, y2;
	vec2f dst, src;

	//top left corner
	src = (vec2f){0.0f, 0.0f};
	dst = mat3x3_V2D_MUL(*trans, src);
	x1 = dst.x; x2 = dst.x;
	y1 = dst.y; y2 = dst.y;

	//top right corner
	src.x = source.w;
	dst = mat3x3_V2D_MUL(*trans, src);
	x1 = MIN(x1, dst.x); x2 = MAX(x2,dst.x);
	y1 = MIN(y1, dst.y); y2 = MAX(y2,dst.y);

	//bottom right corner
	src.y = source.h;
	dst = mat3x3_V2D_MUL(*trans, src);
	x1 = MIN(x1, dst.x); x2 = MAX(x2,dst.x);
	y1 = MIN(y1, dst.y); y2 = MAX(y2,dst.y);

	//bottom left corner
	src.x = 0.0f;
	dst = mat3x3_V2D_MUL(*trans, src);
	x1 = MIN(x1, dst.x); x2 = MAX(x2,dst.x);
	y1 = MIN(y1, dst.y); y2 = MAX(y2,dst.y);

	//clip to target size
	x1 = CLAMP(x1, 0, target.w); x2 = CLAMP(x2, 0, target.w);
	y1 = CLAMP(y1, 0, target.h); y2 = CLAMP(y2, 0, target.h);

	//compute inverted transform matrix
	mat3x3 inv = mat3x3_INV(*trans);

	//scan accross bounding box
	for (int x = x1; x < x2; x++)
	for (int y = y1; y < y2; y++)
	{

		//conver to source space using inverse transform
		src.x = x; src.y = y;
		dst = mat3x3_V2D_MUL(inv, src);

		//skip if outside of the source image
		if (dst.x < 0 || dst.x >= source.w || dst.y < 0 || dst.y >= source.h)
			continue;

		//paint pixel
		EZ_Px* bg = &target.px[x + y * target.w];
		EZ_Px  fg =  source.px[(int)(dst.x) + (int)(dst.y) * source.w];

		_blend(bg, fg);

	}

}