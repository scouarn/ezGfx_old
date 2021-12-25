#include "ezGfx_draw2D.h"



static EZ_BlendMode_t alphaBlending = ALPHA_BLEND;

static void _blend(EZ_Px_t*, EZ_Px_t);


/* alpha blending */
void EZ_draw2D_setBlendMode(EZ_BlendMode_t mode) {
	alphaBlending = mode;
}
/* alpha blending */
static void _blend(EZ_Px_t* px, EZ_Px_t col) {
	*px = EZ_blend(col, *px, alphaBlending);
}


void EZ_draw2D_clear(EZ_Image_t* target, EZ_Px_t col) {
	/* fill everything with one color */
	for (int i = 0; i < target->h * target->w; i++)
		target->px[i] = col;
}


void EZ_draw2D_pixel(EZ_Image_t* target, EZ_Px_t col, int x0, int y0) {

	/* return if ousite of drawing area */
	if (x0 < 0 || x0 >= target->w || y0 < 0 || y0 >= target->h)
		return;

	/* paint pixel */
	EZ_Px_t* px = &(target->px[x0 + y0*target->w]);
	_blend(px, col);
}


static void _VLine(EZ_Image_t* target, EZ_Px_t col, int x, int _y1, int _y2) {
	
	/* return if ousite of drawing area */
	if (x < 0 || x >= target->w) 			return;
	if (_y1 < 0 && _y2 < 0) 				return;
	if (_y1 >= target->h && _y2 >= target->h) return;

	/* clip to fit target */
	int y1 = CLAMP(_y1, 0, target->h-1);
	int y2 = CLAMP(_y2, 0, target->h-1);

	/* draw line */
	for (int h = y1*target->w; h <= y2*target->w; h+=target->w)
		_blend(&target->px[x+h], col);

}

static  void _HLine(EZ_Image_t* target, EZ_Px_t col, int y, int _x1, int _x2) {

	/* return if ousite of drawing area */
	if (y < 0 || y >= target->h) 			return;
	if (_x1 < 0 && _x2 < 0) 				return;
	if (_x1 >= target->w && _x2 >= target->w) return;

	/* clip to fit target */
	int x1 = CLAMP(_x1, 0, target->w-1);
	int x2 = CLAMP(_x2, 0, target->w-1);

	/* draw line */
	int h  = target->w * y;
	for (int x = x1; x <= x2; x++)
		_blend(&target->px[x+h], col);

}

static void _line(EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2) {

	/* dont divide by 0 ! */
	/* x2 != x1 because _VLine would have be called instead */
	/* y2 != y1 because _HLine */

	/* y = slope * x  +  ordog */
	float slope = (float)(y2-y1)/(x2-x1);
	float ordog = y1 - x1 * slope;


	/* clever trick to not end up with a dotted line */
	if (slope > 1.0f || slope < -1.0f) {

		/* clip to fit target */
		int ty1  = CLAMP(MIN(y1, y2), 0, target->h);
		int ty2  = CLAMP(MAX(y1, y2), 0, target->h);

		/* get the value of x at ty1 */
		float x = (ty1 - ordog)/slope;

		/* draw line */
		for (int y = ty1; y <= ty2; y++) {
			EZ_draw2D_pixel(target, col, x, y);
			x += 1.0f/slope;
		}


	}
	else {
		
		/* clip to fit target */
		int tx1  = CLAMP(MIN(x1, x2), 0, target->w);
		int tx2  = CLAMP(MAX(x1, x2), 0, target->w);

		/* get the value of y at tx1 */
		float y = tx1*slope + ordog;

		/* draw line */
		for (int x = tx1; x <= tx2; x++) {
			EZ_draw2D_pixel(target, col, x, y);
			y += slope;
		}

	}



}

void EZ_draw2D_line(EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2) {

	/* draw single point */
	if (x1 == x2 && y1 == y2)
		EZ_draw2D_pixel(target, col, x1, y1);

	/* draw vertical line */
	else if (x1 == x2)
		_VLine(target, col, x1, MIN(y1, y2), MAX(y1, y2));

	/* draw horizontal line */
	else if (y1 == y2)
		_HLine(target, col, y1, MIN(x1, x2), MAX(x1, x2));

	/* draw any other line */
	else
		_line(target, col, x1, y1, x2, y2);


}

void EZ_draw2D_rect(EZ_Image_t* target, EZ_Px_t col, int x0, int y0, int w,  int h) {

	/* return if outside of drawing area */
	if (x0 >= target->w || y0 >= target->h || x0+w < 0 || y0+h < 0) return;

	/* draw for edges */
	/* Hline and Vline already implement clipping */
	_HLine(target, col, y0    , x0  , x0+w-1); /*  top */
	_VLine(target, col, x0    , y0+1, y0+h-2); /*  left */
	_HLine(target, col, y0+h-1, x0  , x0+w-1); /*  bottom */
	_VLine(target, col, x0+w-1, y0+1, y0+h-2); /* right */

}

void EZ_draw2D_fillRect(EZ_Image_t* target, EZ_Px_t col, int x0, int y0, int w,  int h) {
	
	/* return if outside of drawing area */
	if (x0 >= target->w || y0 >= target->h || x0+w < 0 || y0+h < 0) return;

	/* clip to fit target */
	int y1 = MAX(0, y0);
	int y2 = MAX(0, y0+h);

	/* draw horizontal lines */
	for (int y = y1; y < y2; y++)
		_HLine(target, col, y, x0, x0+w-1);

}


void EZ_draw2D_tri(EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2, int x3, int y3) {

	/* draw 3 edges */
	/* clipping implemented in these functions */
	EZ_draw2D_line(target, col, x1, y1, x2, y2);
	EZ_draw2D_line(target, col, x2, y2, x3, y3);
	EZ_draw2D_line(target, col, x3, y3, x1, y1);
}



void EZ_draw2D_fillTri(EZ_Image_t* target, EZ_Px_t col, int x1, int y1, int x2, int y2, int x3, int y3) {

	/* sort points (y1 : top, y2 : mid, y3 : bot) */
	if (y1 > y2) {
		SWAP(y1, y2);
		SWAP(x1, x2);
	}
	if (y1 > y3) {
		SWAP(y1, y3);
		SWAP(x1, x3);
	}
	if (y2 > y3) {
		SWAP(y2, y3);
		SWAP(x2, x3);
	}

	/* compute slopes */
	int dx1 = x2 - x1;
	int dy1 = y2 - y1;

	int dx2 = x3 - x1;
	int dy2 = y3 - y1;

	int dx3 = x3 - x2;
	int dy3 = y3 - y2;

	float dx_start = (float)dx1/dy1;
	float dx_end   = (float)dx2/dy2;


	/* "vertical" clipping */
	int y_start = CLAMP(y1, 0, target->h);
	int y_mid   = CLAMP(y2, 0, target->h);
	int y_end   = CLAMP(y3, 0, target->h);

	/* get values of x for left and right line at y0 */
	float x_start = x1 + (y_start - y1) * dx_start;
	float x_end   = x1 + (y_start - y1) * dx_end;

	int sx, sy;

	/* scan lines */
	for (sy = y_start; sy < y_end; sy++) {

		/* this line actually avoids dividing by zero,
		   by skipping the top triangle when y1 == y2 (same for y2 == y3) */
		if (sy == y_mid) {
			dx_start = (float)dx3/dy3;
			x_start = x2; /* seems to glitch if not corrected */
		}


		/* "horizontal" clipping*/
		int xLeft  = CLAMP(x_start, 0, target->w);
		int xRight = CLAMP(x_end,   0, target->w);

		/* left-right sorting */
		if (xLeft > xRight)
			SWAP(xLeft, xRight);

		/* draw */
		for (sx = xLeft; sx < xRight; sx++)
			target->px[sx + sy * target->w] = col;

		x_start += dx_start; 
		x_end   += dx_end;
	}


}


void EZ_draw2D_ellipse(EZ_Image_t* target, EZ_Px_t col, int x0, int y0, int a,  int b) {

	/* weird algorithm, see : */
	/* https://www.geeksforgeeks.org/midpoint-ellipse-drawing-algorithm/ */

	int x, y;
	float p, dx, dy, a2, b2;


	x = 0; y = b;
	a2 = (float)a*a; b2 = (float)b*b;
	dx = 0; dy = 2.0f * a2 * y;
	p  = b2 - a2*y + 0.25f*a2;

	while (dx < dy && x < target->w && y >= 0) {

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

	while (dx < dy && x < target->w && y >= 0) {

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

void EZ_draw2D_fillEllipse(EZ_Image_t* target, EZ_Px_t col, int x0, int y0, int a,  int b) {

	/* same but with Hlines */
	/* not like EZ_draw2D_pixel */

	int x, y;
	float p, dx, dy, a2, b2;


	x = 0; y = b;
	a2 = (float)a*a; b2 = (float)b*b;
	dx = 0; dy = 2.0f * a2 * y;
	p  = b2 - a2*y + 0.25f*a2;

	while (dx < dy && x < target->w && y >= 0) {


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

	while (dx < dy && x < target->w && y >= 0) {

		if (p <= 0 && x < target->w && y >= 0) {
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



void EZ_draw2D_image(EZ_Image_t* target, EZ_Image_t* source, int x0, int y0) {

	/* return if outside of drawing area */
	if (x0 >= target->w || y0 >= target->h || x0+source->w < 0 || y0+source->h < 0) return;

	/* clip to fit target */
	int x1 = MAX(0, x0);
	int x2 = CLAMP(x0+source->w, 0, target->w);

	int y1 = MAX(0, y0);
	int y2 = CLAMP(y0+source->h, 0, target->h);

	/* scan accross target space */
	for (int x = x1; x < x2; x++) 
	for (int y = y1; y < y2; y++) {
		
		/* convert to source space */
		int sx = x - x0;
		int sy = y - y0;

		/* paint pixel */
		EZ_Px_t  col = source->px[sx + sy*source->w];
		EZ_Px_t* px  = &(target->px[x + y*target->w]);

		_blend(px, col);
	}


}


void EZ_draw2D_croppedImage(EZ_Image_t* target, EZ_Image_t* source, int x0, int y0, int u0, int v0, int w, int h) {

	/* return if outside of drawing area */
	if (x0 >= target->w || y0 >= target->h || x0+w < 0 || y0+h < 0) return;

	/* clip to fit target */
	int x1 = MAX(0, x0);
	int x2 = CLAMP(x0+w, 0, target->w);

	int y1 = MAX(0, y0);
	int y2 = CLAMP(y0+h, 0, target->h);


	/* scan accross target space */
	for (int x = x1; x < x2; x++) 
	for (int y = y1; y < y2; y++) {
	
		/* convert to source space */
		int sx = x - x0 + u0;
		int sy = y - y0 + v0;

		/* paint pixel */
		EZ_Px_t  col = source->px[sx + sy*source->w];
		EZ_Px_t* px  = &(target->px[x + y*target->w]);

		_blend(px, col);

	}

}

void EZ_draw2D_resizedImage(EZ_Image_t* target, EZ_Image_t* source, int x0, int y0, int w, int h) {

	/* return if outside of drawing area */
	if (x0 >= target->w || y0 >= target->h || x0+w < 0 || y0+h < 0) return;

	/* clip to fit target */
	int x1 = MAX(0, x0);
	int x2 = CLAMP(x0+w, 0, target->w);
	int y1 = MAX(0, y0);
	int y2 = CLAMP(y0+h, 0, target->h);


	/* compute x and y scaling factors */
	float xRatio = (float)source->w / w;
	float yRatio = (float)source->h / h;

	/* scan accross target space */
	for (int x = x1; x < x2; x++)
	for (int y = y1; y < y2; y++) {
	
		/* convert to source space	 */
		int sx = (x - x0)*xRatio;
		int sy = (y - y0)*yRatio;

		/* paint pixel */
		EZ_Px_t  col = source->px[sx + sy*source->w];
		EZ_Px_t* px  = &(target->px[x + y*target->w]);

		_blend(px, col);
	}

}


void EZ_draw2D_transformedImage(EZ_Image_t* target, EZ_Image_t* source, EZ_Mat3_t* trans) {


	/* compute target-space bounding box */
	int x1, x2, y1, y2;
	EZ_Vec_t dst, src;

	/* top left corner */
	src = (EZ_Vec_t){{0.0f, 0.0f}};
	EZ_mat3_vdown(&dst, trans, &src);
	x1 = dst.x; x2 = dst.x;
	y1 = dst.y; y2 = dst.y;

	/* top right corner */
	src.x = source->w;
	EZ_mat3_vdown(&dst, trans, &src);
	x1 = MIN(x1, dst.x); x2 = MAX(x2,dst.x);
	y1 = MIN(y1, dst.y); y2 = MAX(y2,dst.y);

	/* bottom right corner */
	src.y = source->h;
	EZ_mat3_vdown(&dst, trans, &src);
	x1 = MIN(x1, dst.x); x2 = MAX(x2,dst.x);
	y1 = MIN(y1, dst.y); y2 = MAX(y2,dst.y);

	/* bottom left corner */
	src.x = 0.0f;
	EZ_mat3_vdown(&dst, trans, &src);
	x1 = MIN(x1, dst.x); x2 = MAX(x2,dst.x);
	y1 = MIN(y1, dst.y); y2 = MAX(y2,dst.y);

	/* clip to target size */
	x1 = CLAMP(x1, 0, target->w); x2 = CLAMP(x2, 0, target->w);
	y1 = CLAMP(y1, 0, target->h); y2 = CLAMP(y2, 0, target->h);

	/* compute inverted transform matrix */
	EZ_Mat3_t inv;
	EZ_mat3_inv(&inv, trans);

	/* scan accross bounding box */
	int x, y;
	for (x = x1; x < x2; x++)
	for (y = y1; y < y2; y++) {

		/* conver to source space using inverse transform */
		src.x = x; src.y = y;
		EZ_mat3_vdown(&dst, &inv, &src);

		/* skip if outside of the source image */
		if (dst.x < 0 || dst.x >= source->w || dst.y < 0 || dst.y >= source->h)
			continue;

		/* paint pixel */
		EZ_Px_t* bg = &target->px[x + y * target->w];
		EZ_Px_t  fg =  source->px[(int)(dst.x) + (int)(dst.y) * source->w];

		_blend(bg, fg);

	}

}