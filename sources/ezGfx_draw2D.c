#include "ezGfx_draw2D.h"


/*
TODO :

test with actual pictures... -> png / bitmap loading

cropped image -> revize the algorithm..
resized image -> fairely easy, see interpolation

"any line" clipping -> kinda easy
triangle clipping  -> see olc

textriangle -> research

*/

enum alphaMode alphaBlending = ALPHA_BLEND;
enum interMode interpolation = INTER_LINEAR;

#define max(a,b) \
 ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })



void EZ_draw2D_alphaMode(enum alphaMode mode) {
  alphaBlending = mode;
}

void EZ_draw2D_interMode(enum interMode mode) {
  interpolation = mode;
}

void EZ_draw2D_clear(EZ_Image* target, EZ_px col) {
  for (int i = 0; i < target->h * target->w; i++)
    target->px[i] = col;
}

void _blend(EZ_px* px, EZ_px col) {

  switch (alphaBlending) {
    case ALPHA_IGNORE : *px = col; break;
    case ALPHA_BLEND  : *px = EZ_blend(col, *px); break;
    case ALPHA_FAST   : *px = (col.col.a == 0) ? *px : col; break;
  }

}

void EZ_draw2D_pixel(EZ_Image* target, EZ_px col, int x1, int y1) {

  if (x1 < 0 || x1 >= target->w || y1 < 0 || y1 >= target->h)
    return;

  EZ_px* px = &(target->px[x1 + y1*target->w]);
  _blend(px, col);
}


void _VLine(EZ_Image* target, EZ_px col, int x, int _y1, int _y2) {
  if (x < 0 || x >= target->w) return;

  int y1 = max(0, _y1);
  int y2 = min(target->h - 1, _y2);

  for (int y = y1; y <= y2; y++)
    EZ_draw2D_pixel(target, col, x, y);

}

void _HLine(EZ_Image* target, EZ_px col, int y, int _x1, int _x2) {

  if (y < 0 || y >= target->h) return;

  int x1 = max(0, _x1);
  int x2 = min(target->w - 1, _x2);

  for (int x = x1; x <= x2; x++)
    EZ_draw2D_pixel(target, col, x, y);

}

void _line(EZ_Image* target, EZ_px col, int _x1, int _y1, int _x2, int _y2) {

  float slope = (float)(_x2-_x1)/(_y2-_y1);

  //clever trick to not end up with a dotted line
  if (slope > 1.0f || slope < -1.0f) {
    slope = 1.0f / slope;
    float y = (_x1 < _x2) ? _y1 : _y2;
    int x1  = min(_x1, _x2);
    int x2  = max(_x1, _x2);

    for (int x = x1; x <= x2; x++) {
      EZ_draw2D_pixel(target, col, x, y);
      y += slope;
    }

  }
  else {
    float x = (_y1 < _y2) ? _x1 : _x2;
    int y1  = min(_y1, _y2);
    int y2  = max(_y1, _y2);

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
    int y1  = min(_y1, _y2);
    int y2  = max(_y1, _y2);
    _VLine(target, col, _x1, y1, y2);
  }

  //horizontal line
  else if (_y1 == _y2) {
    int x1  = min(_x1, _x2);
    int x2  = max(_x1, _x2);
    _HLine(target, col, _y1, x1, x2);

  }

  //any line
  else {
    _line(target, col, _x1, _y1, _x2, _y2);
  }


}

void EZ_draw2D_rect(EZ_Image* target, EZ_px col, int x1, int y1, int h,  int w) {

  if (x1 >= target->w || y1 >= target->h || x1+w < 0 || y1+h < 0) return;

  //Hline and Vline already implement clipping

  _HLine(target, col, y1    , x1  , x1+w-1); // top
  _VLine(target, col, x1    , y1+1, y1+w-2); // left
  _HLine(target, col, y1+w-1, x1  , x1+w-1); // bottom
  _VLine(target, col, x1+w-1, y1+1, y1+w-2); //right

}

void EZ_draw2D_fillRect(EZ_Image* target, EZ_px col, int x0, int y0, int h,  int w) {

  if (x0 >= target->w || y0 >= target->h || x0+w < 0 || y0+h < 0) return;

  int y1 = max(0, y0);
  int y2 = max(0, y0+h-1);

  for (int y = y1; y < y2; y++)
    _HLine(target, col, y, x0, x0+w-1);

}


void EZ_draw2D_tri(EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3) {
  EZ_draw2D_line(target, col, x1, y1, x2, y2);
  EZ_draw2D_line(target, col, x2, y2, x3, y3);
  EZ_draw2D_line(target, col, x3, y3, x1, y1);
}

void _flatTri(EZ_Image* target, EZ_px col, int x1, int x2, int xTop, int yTop, int yBase) {

  float a = min(x1, x2);
  float b = max(x1, x2);

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

    float ratio  = (float)(_y1 - _y2 - 0.5f) / (_y1 - _y3);
    int xSlice = ratio*_x3 + (1.0f - ratio)*_x1;

    _flatTri(target, col, _x2, xSlice, _x1, _y1, _y2 - 1);
    _flatTri(target, col, _x2, xSlice, _x3, _y3, _y2);


  }

}

void EZ_draw2D_texTri  (EZ_Image* target, EZ_px col, int x1, int y1, int x2, int y2, int x3, int y3,
                        EZ_Image* texture,           int u1, int v1, int u2, int v2, int u3, int v3) {

                          //TODO
}



void EZ_draw2D_elli(EZ_Image* target, EZ_px col, int x1, int y1, int a,  int b) {


  //https://www.geeksforgeeks.org/midpoint-ellipse-drawing-algorithm/

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


    EZ_draw2D_pixel(target, col, y1 + y, x1 + x);
    EZ_draw2D_pixel(target, col, y1 - y, x1 + x);
    EZ_draw2D_pixel(target, col, y1 + y, x1 - x);
    EZ_draw2D_pixel(target, col, y1 - y, x1 - x);

    dx += 2.0f * b2;
    x++;
  }




}

void EZ_draw2D_fillElli(EZ_Image* target, EZ_px col, int x1, int y1, int a,  int b) {

  //same but with Hlines

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


    _HLine(target, col, y1 + y, x1 - x, x1 + x);
    _HLine(target, col, y1 - y, x1 - x, x1 + x);

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


    _HLine(target, col, y1 + x, x1 - y, x1 + y);
    _HLine(target, col, y1 - x, x1 - y, x1 + y);

    dx += 2.0f * b2;
    x++;
  }

}



void EZ_draw2D_image(EZ_Image* target, EZ_Image* source, int x0, int y0) {
  // DOES NOT WORK !!!!!
  //
  // if (_x1 >= target->w || y0 >= target->h || x0+source->w < 0 || y0+source->h < 0) return;
  //
  // int x1 = max(0, x0);
  // int x2 = max(0, x0+source->w-1);
  //
  // int y1 = max(0, y0);
  // int y2 = max(0, y0+source->h-1);
  //
  // int targetY = y1*target->w;
  // int sourceY = 0;
  //
  //
  // for (int y = 0; y < y2; y++) {
  //   targetY += target->w;
  //   sourceY += source->w;
  //
  //   for (int x = 0; x < min(source->w, target->w - x1); x++)
  //     _blend(&(target->px[x + x1 + targetY]), source->px[x + sourceY]);
  // }


}


void EZ_draw2D_croppedImage(EZ_Image* target, EZ_Image* source, int x1, int y1, int u1, int v1, int u2, int v2) {
  // DOES NOT WORK !!!!!

  // int targetH = y1*target->w;
  // int sourceH = v1*source->w;
  //
  // for (int y = v1; y < min(source->h, target->h - y1, v2); y++) {
  //   targetH += target->w;
  //   sourceH += source->w;
  //
  //   for (int x = u1; x < min(source->w, target->w - x1, u2); x++)
  //     _blend(&(target->px[x + x1 + targetH]), source->px[x + sourceH]);
  // }

}
void EZ_draw2D_resizedImage(EZ_Image* target, EZ_Image* source, int x1, int y1, int w, int h) {
  //TODO

}
