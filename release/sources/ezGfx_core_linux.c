#include "ezGfx_core.h"

#include <time.h>
#include <pthread.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>


static struct timespec startTime;
static struct timespec lastTime;
static bool running;
static pthread_t thread;
static void* mainThread(void* arg);
static void(*callbacks[_numberOfKeys])(void*param); //array of function pointers

static GLXContext glContext;
static GLuint texture;
static EZ_Image* canvas;
static bool stretched;
static int winWidth;
static int winHeight;
static float canvasX = 1.0f;
static float canvasY = 1.0f;
static void updateBars();

static XVisualInfo* visualInfo;
static Display *disp;
static int screen;
static Window win;
static Atom wm_delete;

static int keyMap(int keyCode);
static EZ_Key keyStates[_numberOfKeys];
static EZ_mouseState mouseState;


void EZ_window(const char* name, int w, int h, EZ_Image* cnvs) {

  winWidth  = w; winHeight  = h;
  canvas = cnvs;

  //init X window
  disp   = XOpenDisplay(NULL);
  screen = DefaultScreen(disp);
  win    = XCreateSimpleWindow(disp, RootWindow(disp, screen), 10, 10, w, h, 1,
                          BlackPixel(disp, screen), BlackPixel(disp, screen));
  EZ_rename(name);
  XSelectInput(disp, win, StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | FocusChangeMask);
  XMapWindow(disp, win);

  wm_delete = XInternAtom(disp, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(disp, win, &wm_delete, 1);


  //init opengl
  GLint GLAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  visualInfo = glXChooseVisual(disp, 0, GLAttribs);
  glContext  = glXCreateContext(disp, visualInfo, NULL, GL_TRUE);

  glXMakeCurrent(disp, win, glContext);
  glViewport(0, 0, w, h);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_TEXTURE_2D);


}

void EZ_start() {
  running = true;
  pthread_create(&thread, NULL, &mainThread, NULL);
}

void EZ_stop() {
  running = false;
}

void EZ_join() {
  pthread_join(thread, NULL);
}

duration EZ_getTime() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  duration time = (now.tv_sec - startTime.tv_sec) + (now.tv_nsec - startTime.tv_nsec) / 1000000000.0;

  return time;
}

EZ_Key* EZ_getKey(enum EZ_KeyCodes code) {
  return &keyStates[code];
}

EZ_mouseState* EZ_getMouse() {
  return &mouseState;
}

void EZ_setCallbak(enum EZ_callbacks clbk, void(*function)(void*param)) {
  callbacks[clbk] = function;
}


void EZ_rename(const char* name) {
  XStoreName(disp, win, name);
}

void EZ_resize(int w, int h) {
  XResizeWindow(disp, win, w, h);
  glViewport(0, 0, w, h);

}

void EZ_setFullscreen(bool val) {
  XEvent xev = {0};
  Atom wm_state   = XInternAtom(disp, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", False);
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = val; // true or false
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;
  XSendEvent(disp, DefaultRootWindow(disp), False,
    SubstructureNotifyMask, &xev);
}

void EZ_setMaximized(bool val) {
  XEvent xev = {0};
  Atom wm_state  =  XInternAtom(disp, "_NET_WM_STATE", False);
  Atom max_horz  =  XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
  Atom max_vert  =  XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_VERT", False);
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = val; // true or false
  xev.xclient.data.l[1] = max_horz;
  xev.xclient.data.l[2] = max_vert;
  XSendEvent(disp, DefaultRootWindow(disp), False,
    SubstructureNotifyMask, &xev);
}

void EZ_setStretching(bool val) {
  stretched = val;
}


EZ_Image* EZ_createImage(int w, int h) {
  EZ_Image* img = malloc(sizeof(EZ_Image));
  img->w = w;
  img->h = h;
  img->px = calloc(w*h, sizeof(EZ_px));

  return img;
}

EZ_Image* EZ_copyImage(EZ_Image* source) {

  EZ_Image* img = malloc(sizeof(EZ_Image));
  img->w = source->w;
  img->h = source->h;

  img->px = malloc(img->w*img->h*sizeof(EZ_px));
  memcpy(img->px, source->px, img->w*img->h*sizeof(EZ_px));

  return img;
}

void EZ_freeImage(EZ_Image* img) {
  free(img->px);
  free(img);
}

EZ_px EZ_colorRGB(int r, int g, int b) {
  EZ_px col;
  col.col.r = r; col.col.g = g; col.col.b = b; col.col.a = 255;
  return col;
}

EZ_px EZ_randCol() {
  EZ_px col;
  col.ref = (long)rand() << 8;
  return col;
}

EZ_px EZ_blend(EZ_px colA, EZ_px colB) {
  EZ_px colC;

  float aA = colA.col.a / 255.0f;
  float aB = colB.col.a / 255.0f;

  colC.col.a = (aA + aB * (1.0f - aA)) * 255;
  colC.col.r = aA * colA.col.r + (1.0f - aA) * colB.col.r;
  colC.col.g = aA * colA.col.g + (1.0f - aA) * colB.col.g;
  colC.col.b = aA * colA.col.b + (1.0f - aA) * colB.col.b;

  return colC;
}


void EZ_redraw() {

  //time
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  duration elapsedTime = (now.tv_sec - lastTime.tv_sec) + (now.tv_nsec - lastTime.tv_nsec) / 1000000000.0;
  lastTime = now;

  //user function
  if (callbacks[ON_DRAW]) callbacks[ON_DRAW](&elapsedTime);


  //display
  glClear( GL_COLOR_BUFFER_BIT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas->w, canvas->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, canvas->px);


  //draw texture
  glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-canvasX, -canvasY);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-canvasX,  canvasY);
			glTexCoord2f(1.0f, 0.0f); glVertex2f( canvasX,  canvasY);
			glTexCoord2f(1.0f, 1.0f); glVertex2f( canvasX, -canvasY);
	glEnd();


  glXSwapBuffers(disp, win);

}






/* PRIVATE */

/*
DOESNT WORK WITH KEYMAPS
static int keyMap(int keyCode) {
  //https://cgit.freedesktop.org/xorg/proto/x11proto/tree/keysymdef.h

  switch (keyCode) {
    case XK_space :       return K_SPACE;   case XK_BackSpace :   return K_BACKSPACE;
    case XK_Escape :      return K_ESCAPE;  case XK_Tab :         return K_TAB;
    case XK_Linefeed :    return K_RETURN;  case XK_Return :      return K_RETURN;
    case XK_Shift_L :     return K_LSHIFT;  case XK_Shift_R :     return K_RSHIFT;
    case XK_Control_L :   return K_LCTRL;   case XK_Control_R :   return K_RCTRL;
    case XK_Alt_L :       return K_LALT;    case XK_Alt_R :       return K_RALT;
    case XK_Caps_Lock :   return K_CAPS;    case XK_Num_Lock :    return K_NUMLOCK;
    case XK_Scroll_Lock : return K_SCROLL;  case XK_Pause :       return K_PAUSE;
    case XK_Insert :      return K_INS;     case XK_Delete :      return K_DEL;
    case XK_Home :        return K_HOME;    case XK_End :         return K_END;
    case XK_Page_Up :     return K_PGUP;    case XK_Page_Down :   return K_PGDN;
    case XK_Left :        return K_LEFT;    case XK_Up :          return K_UP;
    case XK_Right :       return K_RIGHT;   case XK_Down :        return K_DOWN;
    case XK_KP_Enter :    return K_RETURN;  case XK_period :      return K_PERIOD;
    case XK_KP_Multiply : return KP_MUL;    case XK_KP_Divide :   return KP_DIV;
    case XK_KP_Add :      return K_PLUS;    case XK_KP_Subtract : return K_MINUS;
    case XK_semicolon :   return K_COLON;   case XK_comma:        return K_COMMA;
    case XK_apostrophe :  return K_QUOTE;   case XK_grave :       return K_TILDE;
    case XK_slash :       return K_SLASH;   case XK_backslash:    return K_BACKSLASH;
    case XK_bracketleft : return K_OPEN;    case XK_bracketright: return K_CLOSE;
    case XK_equal :       return K_PLUS;    case XK_minus:        return K_MINUS;


    case XK_KP_Insert :   return KP_0;
    case XK_KP_End :      return KP_1;
    case XK_KP_Down :     return KP_2;
    case XK_KP_Next :     return KP_3;
    case XK_KP_Left :     return KP_4;
    case XK_KP_Begin :    return KP_5;
    case XK_KP_Right :    return KP_6;
    case XK_KP_Home :     return KP_7;
    case XK_KP_Up :       return KP_8;
    case XK_KP_Prior :    return KP_9;
    case XK_KP_Delete :   return KP_DEC;

    default :
      if      (keyCode >= XK_a && keyCode <= XK_z)       { return keyCode - XK_a  + K_A; }
      else if (keyCode >= XK_0 && keyCode <= XK_9)       { return keyCode - XK_0  + K_0; }  //Not working right
      else if (keyCode >= XK_F1 && keyCode <= XK_F12)    { return keyCode - XK_F1 + K_F1; }
      else return K_ERROR;
  }

}
*/

static int keyMap(int keyCode) {
  //https://cgit.freedesktop.org/xorg/proto/x11proto/tree/keysymdef.h

  switch (keyCode) {
    case 0x41 : return K_SPACE;   case 0x16 : return K_BACKSPACE;
    case 0x09 : return K_ESCAPE;  case 0x17 : return K_TAB;
    case 0x24 : return K_RETURN;
    case 0x32 : return K_LSHIFT;  case 0x3e : return K_RSHIFT;
    case 0x25 : return K_LCTRL;   case 0x69 : return K_RCTRL;
    case 0x40 : return K_LALT;    case 0x6c : return K_RALT;
    case 0x42 : return K_CAPS;    case 0x4d : return K_NUMLOCK;
    case 0x4e : return K_SCROLL;  case 0x7f : return K_PAUSE;
    case 0x76 : return K_INS;     case 0x77 : return K_DEL;
    case 0x6e : return K_HOME;    case 0x73 : return K_END;
    case 0x70 : return K_PGUP;    case 0x75 : return K_PGDN;
    case 0x71 : return K_LEFT;    case 0x6f : return K_UP;
    case 0x72 : return K_RIGHT;   case 0x74 : return K_DOWN;

    case 0x3c : return K_PERIOD;
    case 0x2f : return K_COLON;   case 0x3b : return K_COMMA;
    case 0x30 : return K_QUOTE;   case 0x31 : return K_TILDE;
    case 0x3d : return K_SLASH;   case 0x33 :  return K_BACKSLASH;
    case 0x22 : return K_OPEN;    case 0x23 : return K_CLOSE;
    case 0x15 : return K_PLUS;    case 0x14 : return K_MINUS;

    case 0x5b : return KP_DEC;  case 0x68 : return K_RETURN;
    case 0x3f : return KP_MUL;  case 0x6a : return KP_DIV;
    case 0x56 : return K_PLUS;  case 0x52 : return K_MINUS;

    case 0x5a : return KP_0;  case 0x57 : return KP_1;  case 0x58 : return KP_2;
    case 0x59 : return KP_3;  case 0x53 : return KP_4;  case 0x54 : return KP_5;
    case 0x55 : return KP_6;  case 0x4f : return KP_7;  case 0x50 : return KP_8;
    case 0x51 : return KP_9;

    case 0x13 : return K_0;   case 0x0a : return K_1;   case 0x0b : return K_2;
    case 0x0c : return K_3;   case 0x0d : return K_4;   case 0x0e : return K_5;
    case 0x0f : return K_6;   case 0x10 : return K_7;   case 0x11 : return K_8;
    case 0x12 : return K_9;

    case 0x43 : return K_F1;  case 0x44 : return K_F2;  case 0x45 : return K_F3;
    case 0x46 : return K_F4;  case 0x47 : return K_F5;  case 0x48 : return K_F6;
    case 0x49 : return K_F7;  case 0x4a : return K_F8;  case 0x4b : return K_F9;
    case 0x4c : return K_F10; case 0x5f : return K_F11; case 0x60 : return K_F12;

    case 0x18 : return K_Q;   case 0x19 : return K_W;   case 0x1a : return K_E;
    case 0x1b : return K_R;   case 0x1c : return K_T;   case 0x1d : return K_Y;
    case 0x1e : return K_U;   case 0x1f : return K_I;   case 0x20 : return K_O;
    case 0x21 : return K_P;   case 0x26 : return K_A;   case 0x27 : return K_S;
    case 0x28 : return K_D;   case 0x29 : return K_F;   case 0x2a : return K_G;
    case 0x2b : return K_H;   case 0x2c : return K_J;   case 0x2d : return K_K;
    case 0x2e : return K_L;   case 0x34 : return K_Z;   case 0x35 : return K_X;
    case 0x36 : return K_C;   case 0x37 : return K_V;   case 0x38 : return K_B;
    case 0x39 : return K_N;   case 0x3a : return K_M;

    default : return K_ERROR;
  }
}

static void updateBars() {

  //black bars

  if (stretched) {
    canvasX = 1.0f;
    canvasY = 1.0f;
  }
  else {

    float canvasRatio = (float)canvas->w / canvas->h;
    float windowRatio = (float)winWidth / winHeight;
    canvasX = canvasRatio / windowRatio;
    canvasY = windowRatio / canvasRatio;

    if (canvasX > 1.0f) {
      canvasX = 1.0f;
    }
    else {
      canvasY = 1.0f;
    }


  }

}


static void* mainThread(void* arg) {

   pthread_detach(pthread_self());

   clock_gettime(CLOCK_MONOTONIC, &startTime);
   clock_gettime(CLOCK_MONOTONIC, &lastTime);

   if (callbacks[ON_CREATE]) callbacks[ON_CREATE](NULL);

   //main loop
   while (running) {

      if (disp) {

        //reset keystate stuff
        for (int i = 0; i < _numberOfKeys; i++) {
          keyStates[i].pressed  = false;
          keyStates[i].released = false;
        }

        //events and inputs
        while (XPending(disp)) {
          XEvent e;
          XNextEvent(disp, &e);

          switch (e.type) {
            case Expose:
              break;

            case KeyPress:
            {
              int index = keyMap(e.xkey.keycode);
              EZ_Key* key = &keyStates[index];

              key->pressed = true;
              key->held    = true;
              XLookupString(&e.xkey, &key->typed, 1, NULL, NULL);

              if (callbacks[ON_KEYPRESSED]) callbacks[ON_KEYPRESSED](&index);
              break;
            }

            case KeyRelease:
            {
              int index = keyMap(e.xkey.keycode);
              EZ_Key* key = &keyStates[index];

              key->released = true;
              key->held = false;
              XLookupString(&e.xkey, &key->typed, 1, NULL, NULL);

              if (callbacks[ON_KEYRELEASED]) callbacks[ON_KEYRELEASED](&index);
              break;
            }

            case FocusIn :
            {
              XAutoRepeatOff(disp); //turn off autistic keyboard inputs
              break;
            }

            case FocusOut:
            {
              XAutoRepeatOn(disp); //put it back on so other apps are not affected
              break;
            }

            case ButtonPress:
              {
              int sym = e.xbutton.button;
              int index = 0;
              switch (sym)
      					{
      					case 1:	keyStates[K_LMB].pressed = true; keyStates[K_LMB].held = true; index = 1; break;
      					case 2:	keyStates[K_MMB].pressed = true; keyStates[K_MMB].held = true; index = 3; break;
      					case 3:	keyStates[K_RMB].pressed = true; keyStates[K_RMB].held = true; index = 2; break;
      					case 4:	mouseState.wheel = 1; break;
      					case 5:	mouseState.wheel =-1; break;
      					default: break;
      					}
              if (callbacks[ON_KEYPRESSED]) callbacks[ON_KEYPRESSED](&index);
              break;
              }

            case ButtonRelease:
              {
              int sym = e.xbutton.button;
              int index = 0;
              switch (sym)
                {
                case 1:	keyStates[K_LMB].released = true; keyStates[K_LMB].held = false; index = 1; break;
                case 2:	keyStates[K_MMB].released = true; keyStates[K_MMB].held = false; index = 3; break;
                case 3:	keyStates[K_RMB].released = true; keyStates[K_RMB].held = false; index = 2; break;
                default: break;
                }
              if (callbacks[ON_KEYRELEASED]) callbacks[ON_KEYRELEASED](&index);
              break;
              }

            case MotionNotify:
            {

              int newx = (e.xmotion.x + 0.5f*(canvasX - 1.0f) * winWidth ) * canvas->w / ((float)winWidth  * canvasX);
              int newy = (e.xmotion.y + 0.5f*(canvasY - 1.0f) * winHeight) * canvas->h / ((float)winHeight * canvasY);

              mouseState.dx = newx - mouseState.x;
              mouseState.dy = newy - mouseState.y;
              mouseState.x  = newx;
              mouseState.y  = newy;

              if (callbacks[ON_MOUSEMOVE]) callbacks[ON_MOUSEMOVE](NULL);

              break;
            }

            case ConfigureNotify: //resize
            {
              XConfigureEvent req = e.xconfigure;
              winWidth = req.width; winHeight = req.height;
              glViewport(0, 0, req.width, req.height);
              updateBars();

              break;
            }

            case ClientMessage: //on destroy
            {
              if ((Atom)e.xclient.data.l[0] == wm_delete)
                EZ_stop();

              break;
            }


          }
        }

        //display
        EZ_redraw();

      }



   }


   if (callbacks[ON_CLOSE]) callbacks[ON_CLOSE](NULL);

   //free everything
   glXMakeCurrent(disp, None, NULL);
   glXDestroyContext(disp, glContext);
   XAutoRepeatOn(disp); //put it back on so other apps are not affected
   XCloseDisplay(disp);
   pthread_exit(NULL);

   return NULL;
}
