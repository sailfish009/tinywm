// tinywm, 8l, zlib license, 2016-2018
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <algorithm>
#include <vector>
#include <stdio.h>

#define SCREEN_WIDTH   (1920 * 2)
#define SCREEN_HEIGHT  1080
#define SIDE_WIN_RESIZE 0

#define SET_KEY(dpy, c) \
  XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym(c)), Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync)

#define SET_KEY2(dpy, c) \
  XGrabKey(dpy, XKeysymToKeycode(dpy, c), Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync)

#define SET_KEY3(dpy, a, b) \
  XGrabButton(dpy, a, b, DefaultRootWindow(dpy), True, \
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None)

Display * dpy;
XEvent ev;
Window w = None, zoom = None, focus = None, root = None;
XWindowAttributes attr;
XButtonEvent start{0};
unsigned short rect[4]={0};
bool b_close = 0;
bool list_toggle;

void SetInput(Display *dpy)
{
  root = RootWindow(dpy, DefaultScreen(dpy));
  SET_KEY(dpy, "a");  SET_KEY(dpy, "c");  SET_KEY(dpy, "f");  SET_KEY(dpy, "h");  
  SET_KEY(dpy, "r");  SET_KEY(dpy, "t");  SET_KEY(dpy, "x");  SET_KEY(dpy, "z");
  SET_KEY2(dpy, XK_Escape); SET_KEY2(dpy, XK_Tab);
  SET_KEY3(dpy, 1, Mod1Mask); SET_KEY3(dpy, 3, Mod1Mask); SET_KEY3(dpy, 8, 0); SET_KEY3(dpy, 9, 0);
  XSetWindowBackground(dpy, root, 0x0192c6);
  XClearWindow(dpy, root);

  system("xclock &");
  system("term &");
  system("sct 2300");
}

inline void ProcessKey(const XEvent &ev)
{
  switch(ev.xkey.keycode)
  {
  case 27:
    system("dmenu_run &");
    break;
  case 28:
    system("term &");
    break;
  case 41:
    system("firefox &");
    break;
  case 43:
    if(ev.xkey.subwindow != None && w == None)
    {
      w = ev.xkey.subwindow;
      XUnmapWindow(dpy, ev.xkey.subwindow);
    }
    else 
    if(w != None)
    {
      XMapWindow(dpy, w);
      w = None;
    }
    break;
  case 52:
    if(ev.xkey.subwindow != None && zoom == None)
    {
      zoom = ev.xkey.subwindow;
      XGetWindowAttributes(dpy, zoom, &attr);
      rect[0]=attr.x;
      rect[1]=attr.y;
      rect[2]=attr.width;
      rect[3]=attr.height;
      XMoveResizeWindow(dpy,zoom,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    }
    else
    if(zoom != None)
    {
      XMoveResizeWindow(dpy,zoom,rect[0],rect[1],rect[2],rect[3]);
      zoom = None;
    }
    break;
  case 53:
    if(ev.xkey.subwindow != None && b_close == 0)
    {
      b_close = 1;
      XKillClient(dpy, ev.xkey.subwindow);
    }
    break;
  case 54:
    system("chrome &");
    break;
  case 9:
    // log out
    // system("gnome-session-quit");
    // screen lock
    system("mate-screensaver-command -l");
    break;
  }
}

inline void ProcessMouse(const XEvent &ev)
{
  switch(ev.type)
  {
  case ButtonPress:
    if(ev.xbutton.subwindow != None)
    {
      switch(ev.xbutton.button)
      {
      //page up
      case 8:
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Up), 1, CurrentTime);
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Up), 0, CurrentTime);
      break;
      //page down
      case 9:
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Down), 1, CurrentTime);
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Down), 0, CurrentTime);
      break;

      default:
      focus = ev.xbutton.subwindow; 
      XRaiseWindow(dpy, focus);
      XSetInputFocus(dpy, focus, RevertToParent, CurrentTime);
      XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
      start = ev.xbutton;
      break;
      }
    }
    break;
  case ButtonRelease:
    {
      start.subwindow = None;
#ifdef SIDE_WIN_RESIZE
      if(ev.xbutton.x_root > SCREEN_WIDTH - 5)
        XMoveResizeWindow(dpy,ev.xbutton.subwindow,SCREEN_WIDTH*2/3,0,SCREEN_WIDTH*1/3,SCREEN_HEIGHT);
      else if(ev.xbutton.x_root < 5)
        XMoveResizeWindow(dpy,ev.xbutton.subwindow,0,0,SCREEN_WIDTH*2/3,SCREEN_HEIGHT);
#endif
    }
    break;
  case MotionNotify:
    if(start.subwindow != None)
    {
      int xdiff = ev.xbutton.x_root - start.x_root;
      int ydiff = ev.xbutton.y_root - start.y_root;
      XMoveResizeWindow(dpy, start.subwindow,
        attr.x + (start.button==1 ? xdiff : 0),
        attr.y + (start.button==1 ? ydiff : 0),
        std::max(1, attr.width + (start.button==3 ? xdiff : 0)),
        std::max(1, attr.height + (start.button==3 ? ydiff : 0)));     

      XEvent event;
      event.type = Expose;
      event.xexpose.window = root;
      XSendEvent(dpy, root, False, ExposureMask, &event);
      XSync(dpy, 0);
    }
    break;
  }
}

