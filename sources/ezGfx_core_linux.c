#include "ezGfx_core.h"

#include <time.h>
#include <pthread.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>


struct timespec startTime;
struct timespec lastTime;
bool running;
pthread_t thread;
void* mainThread(void* arg);

GLXContext glContext;
GLuint texture;

bool stretched;
int winWidth;  int winHeight;
float canvasX = 1.0f; float canvasY = 1.0f;

int keyMap(int keyCode);

XVisualInfo* visualInfo;
Display *disp;
int screen;
Window win;
Atom wm_delete;



void EZ_window(const char* name, int w, int h) {

  winWidth  = w; winHeight  = h;

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
  if (EZ_callbacks[ON_DRAW]) EZ_callbacks[ON_DRAW](&elapsedTime);


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


int keyMap(int keyCode) {

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
      else if (keyCode >= XK_0 && keyCode <= XK_9)       { return keyCode - XK_0  + K_0; }
      else if (keyCode >= XK_F1 && keyCode <= XK_F12)    { return keyCode - XK_F1 + K_0; }
      else return K_ERROR;
  }

}


void updateBars() {

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


void* mainThread(void* arg) {

   pthread_detach(pthread_self());

   clock_gettime(CLOCK_MONOTONIC, &startTime);
   clock_gettime(CLOCK_MONOTONIC, &lastTime);

   if (EZ_callbacks[ON_CREATE]) EZ_callbacks[ON_CREATE](NULL);

   //main loop
   while (running) {

      if (disp) {

        //reset keystate stuff
        for (int i = 0; i < _numberOfKeys; i++) {
          EZ_keyStates[i].pressed  = false;
          EZ_keyStates[i].released = false;
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
              KeySym sym = XLookupKeysym(&e.xkey, 0);
              EZ_Key* key = &EZ_keyStates[keyMap(sym)];

              key->pressed = true;
              key->held    = true;

              if (EZ_callbacks[ON_KEYPRESSED]) EZ_callbacks[ON_KEYPRESSED](NULL);
              break;
            }

            case KeyRelease:
            {
              KeySym sym = XLookupKeysym(&e.xkey, 0);
              EZ_Key* key = &EZ_keyStates[keyMap(sym)];

              key->released = true;
              key->held = false;

              if (EZ_callbacks[ON_KEYRELEASED]) EZ_callbacks[ON_KEYRELEASED](NULL);
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
              switch (e.xbutton.button)
      					{
      					case 1:	EZ_keyStates[K_LMB].pressed = true; EZ_keyStates[K_LMB].held = true; break;
      					case 2:	EZ_keyStates[K_MMB].pressed = true; EZ_keyStates[K_MMB].held = true; break;
      					case 3:	EZ_keyStates[K_RMB].pressed = true; EZ_keyStates[K_RMB].held = true; break;
      					case 4:	EZ_mouse.wheel = 1; break;
      					case 5:	EZ_mouse.wheel =-1; break;
      					default: break;
      					}
              if (EZ_callbacks[ON_KEYPRESSED]) EZ_callbacks[ON_KEYPRESSED](NULL);
              break;

            case ButtonRelease:
              switch (e.xbutton.button)
                {
                case 1:	EZ_keyStates[K_LMB].released = true; EZ_keyStates[K_LMB].held = false; break;
                case 2:	EZ_keyStates[K_MMB].released = true; EZ_keyStates[K_MMB].held = false; break;
                case 3:	EZ_keyStates[K_RMB].released = true; EZ_keyStates[K_RMB].held = false; break;
                default: break;
                }
              if (EZ_callbacks[ON_KEYRELEASED]) EZ_callbacks[ON_KEYRELEASED](NULL);
              break;


            case MotionNotify:
            {

              int newx = (e.xmotion.x + 0.5f*(canvasX - 1.0f) * winWidth ) * canvas->w / ((float)winWidth  * canvasX);
              int newy = (e.xmotion.y + 0.5f*(canvasY - 1.0f) * winHeight) * canvas->h / ((float)winHeight * canvasY);

              EZ_mouse.dx = newx - EZ_mouse.x;
              EZ_mouse.dy = newy - EZ_mouse.y;
              EZ_mouse.x  = newx;
              EZ_mouse.y  = newy;

              if (EZ_callbacks[ON_MOUSEMOVE]) EZ_callbacks[ON_MOUSEMOVE](NULL);

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


   if (EZ_callbacks[ON_CLOSE]) EZ_callbacks[ON_CLOSE](NULL);

   //free everything
   EZ_freeImage(canvas);
   glXMakeCurrent(disp, None, NULL);
   glXDestroyContext(disp, glContext);
   XAutoRepeatOn(disp); //put it back on so other apps are not affected
   XCloseDisplay(disp);
   pthread_exit(NULL);

   return NULL;
}
