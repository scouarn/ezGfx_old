#include "ezGfx_draw2D.h"

#include <math.h>


enum alphaMode alphaBlending = ALPHA_BLEND;
enum interMode interpolation = INTER_LINEAR;

void EZ_draw2D_alphaMode(enum alphaMode mode) {
  alphaBlending = mode;
}

void EZ_draw2D_interMode(enum alphaMode mode) {
  interpolation = mode;
}

void EZ_draw2D_clear(EZ_Image* target, EZ_px col) {
  for (int i = 0; i < target->h * target->w; i++)
    target->px[i] = col;
}

void EZ_draw2D_pixel(EZ_Image* target, EZ_px col, int x1, int y1) {

  if (x1 < 0 || x1 >= target->w || y1 < 0 || y1 >= target->h)
    return;

  EZ_px* px = &(target->px[x1 + y1*target->w]);

  switch (alphaBlending) {
    case ALPHA_IGNORE : *px = col; break;
    case ALPHA_BLEND  : *px = EZ_blend(col, *px); break;
    case ALPHA_FAST   : *px = (col.col.a == 0) ? *px : col; break;
  }

}


void _VLine(EZ_Image* target, EZ_px col, int x, int y1, int y2) {
  for (int y = y1; y <= y2; y++)
    EZ_draw2D_pixel(target, col, x, y);
}

void _HLine(EZ_Image* target, EZ_px col, int y, int x1, int x2) {
  for (int x = x1; x <= x2; x++)
    EZ_draw2D_pixel(target, col, x, y);
}

void _line(EZ_Image* target, EZ_px col, int _x1, int _y1, int _x2, int _y2) {

  float slope = (float)(_x2-_x1)/(_y2-_y1);

  //clever trick to not end up with a dotted line
  if (slope > 1.0f || slope < -1.0f) {
    slope = 1.0f / slope;
    float y = (_x1 < _x2) ? _y1 : _y2;
    int x1  = (_x1 < _x2) ? _x1 : _x2;
    int x2  = (_x1 > _x2) ? _x1 : _x2;

    for (int x = x1; x <= x2; x++) {
      EZ_draw2D_pixel(target, col, x, y);
      y += slope;
    }

  }
  else {
    float x = (_y1 < _y2) ? _x1 : _x2;
    int y1  = (_y1 < _y2) ? _y1 : _y2;
    int y2  = (_y1 > _y2) ? _y1 : _y2;

    for (int y = y1; y <= y2; y++) {
      EZ_draw2D_pixel(target, col, x, y);
      x += slope;
    }

  }



}

void EZ_draw2D_line(EZ_Image* target, EZ_px col, int _x1, int _y1, int _x2, int _y2) {

  //point
  if (_x1 == _x2 && _y1 == _y2) {
    EZ_draw2D_pixel(target, col, _x1, _y1);
  }

  //vertical line
  else if (_x1 == _x2) {
    int y1 = (_y1 < _y2) ? _y1 : _y2;
    int y2 = (_y1 > _y2) ? _y1 : _y2;
    _VLine(target, col, _x1, y1, y2);
  }

  //horizontal line
  else if (_y1 == _y2) {
    int x1 = (_x1 < _x2) ? _x1 : _x2;
    int x2 = (_x1 > _x2) ? _x1 : _x2;
    _HLine(target, col, _y1, x1, x2);

  }

  //any line
  else {
    _line(target, col, _x1, _y1, _x2, _y2);
  }


}

void EZ_draw2D_rect(EZ_Image* target, EZ_px col, int x1, int y1, int h,  int w) {

  _HLine(target, col, y1, x1, x1+w-1);       // top
  _HLine(target, col, y1+h-1, x1, x1+w-1);   // bottom
  _VLine(target, col, x1, y1+1, y1+h-2);     // left
  _VLine(target, col, x1+w-1, y1+1, y1+h-2); // right

}

void EZ_draw2D_fillRect(EZ_Image* target, EZ_px col, int x1, int y1, int h,  int w) {

  for (int y = y1; y < y1+h; y++)
    _HLine(target, col, y, x1, x1+w-1);

}


void EZ_draw2D_tri(EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3) {
  _line(target, col, x1, y1, x2, y2);
  _line(target, col, x2, y2, x3, y3);
  _line(target, col, x3, y3, x1, y1);
}

void _flatTri(EZ_Image* target, EZ_px col, int x1, int x2, int xTop, int yTop, int yBase) {

  float a = x1 < x2 ? x1 : x2;
  float b = x1 > x2 ? x1 : x2;

  float LSlope = (float)(xTop-x1)/(yBase-yTop);
  float RSlope = (float)(xTop-x2)/(yBase-yTop);

  if (yBase > yTop)
    for (int y = yBase; y >= yTop; y--) {
      _HLine(target, col, y, a, b);

      a += LSlope;
      b += RSlope;
    }
  else for (int y = yBase; y <= yTop; y++) {
      _HLine(target, col, y, a, b);

      a -= LSlope;
      b -= RSlope;
    }

}


void EZ_draw2D_fillTri(EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3) {


  if (y1 == y2)
    _flatTri(target, col, x1, x2, x3, y3, y1);

  else if (y2 == y3)
    _flatTri(target, col, x2, x3, x1, y1, y3);

  else {

    //sort points (y1 : top, y2 : mid, y3 : bot)
    int _y1 = y1; int _y2 = y2; int _y3 = y3;
    int _x1 = x1; int _x2 = x2; int _x3 = x3;

    if (_y1 > _y2) {
      int t;
      t = _y1; _y1 = _y2; _y2 = t;
      t = _x1; _x1 = _x2; _x2 = t;
    }
    if (_y1 > _y3) {
      int t;
      t = _y1; _y1 = _y3; _y3 = t;
      t = _x1; _x1 = _x3; _x3 = t;
    }
    if (_y2 > _y3) {
      int t;
      t = _y2; _y2 = _y3; _y3 = t;
      t = _x2; _x2 = _x3; _x3 = t;
    }

    float ratio = (float)(_y1 - _y2) / (_y1 - _y3);
    int xSlice  = ratio*_x3 + (1.0f - ratio)*_x1;

    _flatTri(target, col, _x2, xSlice, _x1, _y1, _y2);
    _flatTri(target, col, _x2, xSlice, _x3, _y3, _y2);


  }

}


void EZ_draw2D_elli(EZ_Image* target, EZ_px col, int x1, int y1, int a,  int b) {

  // //NAIVE
  // float angle = 0;
  // while (angle < 1.5707964) {
  //   float x = a*cosf(angle);
  //   float y = b*sinf(angle);
  //
  //   EZ_draw2D_pixel(target, EZ_RED, x1 + x, y1 + y);
  //   EZ_draw2D_pixel(target, EZ_RED, x1 + x, y1 - y);
  //   EZ_draw2D_pixel(target, EZ_RED, x1 - x, y1 + y);
  //   EZ_draw2D_pixel(target, EZ_RED, x1 - x, y1 - y);
  //
  //   angle += 0.01f;
  // }

  //https://www.geeksforgeeks.org/midpoint-ellipse-drawing-algorithm/

  int x, y;
  float p, dx, dy, a2, b2;


  x = 0; y = b;
  a2 = (float)a*a; b2 = (float)b*b;
  dx = 0; dy = 2.0f * a2 * y;
  p  = b2 - a2*y + 0.25f*a2;

  while (dx < dy) {


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

  while (dx < dy) {

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

void EZ_draw2D_fillElli(EZ_Image* target, EZ_px col, int x1, int y1, int a,  int b) {
  //NAIVE
  // float angle = 0;
  // while (angle < 1.5707964) {
  //   float x = a*cos(angle);
  //   float y = b*sin(angle);
  //
  //   _HLine(target, col, y1 + y, x1 - x, x1 + x);
  //   _HLine(target, col, y1 - y, x1 - x, x1 + x);
  //
  //   angle += 0.1f;
  // }

  int x, y;
  float p, dx, dy, a2, b2;


  x = 0; y = b;
  a2 = (float)a*a; b2 = (float)b*b;
  dx = 0; dy = 2.0f * a2 * y;
  p  = b2 - a2*y + 0.25f*a2;

  while (dx < dy) {


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

  while (dx < dy) {

    if (p <= 0) {
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
