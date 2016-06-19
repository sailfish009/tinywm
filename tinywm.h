#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>

Display * dpy;
Window w = None;
XWindowAttributes attr;
XButtonEvent start{0};
KeyCode CK, XK, RK, HK, FK, EK;

void SetInput(Display *dpy)
{
  CK = XKeysymToKeycode(dpy, XStringToKeysym("c"));
  XK = XKeysymToKeycode(dpy, XStringToKeysym("x"));
  RK = XKeysymToKeycode(dpy, XStringToKeysym("r"));
  HK = XKeysymToKeycode(dpy, XStringToKeysym("h"));
  FK = XKeysymToKeycode(dpy, XStringToKeysym("f"));
  EK = XKeysymToKeycode(dpy, XK_Escape);

  XGrabKey(dpy, CK, Mod1Mask,   DefaultRootWindow(dpy), True, 
    GrabModeAsync, GrabModeAsync);
    
  XGrabKey(dpy, XK, Mod1Mask,   DefaultRootWindow(dpy), True, 
    GrabModeAsync, GrabModeAsync);

  XGrabKey(dpy, RK, Mod1Mask,   DefaultRootWindow(dpy), True, 
    GrabModeAsync, GrabModeAsync);

  XGrabKey(dpy, HK, Mod1Mask,   DefaultRootWindow(dpy), True, 
    GrabModeAsync, GrabModeAsync);

  XGrabKey(dpy, FK, Mod1Mask,   DefaultRootWindow(dpy), True, 
    GrabModeAsync, GrabModeAsync);

  XGrabKey(dpy, EK, Mod1Mask,   DefaultRootWindow(dpy), True, 
    GrabModeAsync, GrabModeAsync);

  XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

  XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
}

inline int ProcessKey(const XEvent &ev, bool *close)
{
  printf("mod: %d, key: %d\n", ev.xkey.state, ev.xkey.keycode);
  switch(ev.xkey.state + ev.xkey.keycode)
  {
  //alt + r : 8 + 27
  case 35:
    system("dmenu_run &");
    break;
  //alt + f : 8 + 41
  case 49:
    if(ev.xkey.subwindow != None)
      XRaiseWindow(dpy, ev.xkey.subwindow); 
    break;
  //alt + h : 8 + 43
  case 51:
    if(ev.xkey.subwindow != None && w == None)
    {
      w = ev.xkey.subwindow;
      XUnmapWindow(dpy, ev.xkey.subwindow);
    }
    else 
    if(ev.xkey.subwindow == None && w != None)
    {
      XMapWindow(dpy, w);
      w = None;
    }
    break;
  //alt + x : 8 + 53
  case 61:
    if(ev.xkey.subwindow != None && *close == 0)
    {
      *close = 1;
      XKillClient(dpy, ev.xkey.subwindow);
    }
    break;
  //alt + esc : 8 + 9
  case 17:
    return 0;
  }
  
  return 1;
}

inline void ProcessMouse(const XEvent &ev)
{
  switch(ev.type)
  {
  case ButtonPress:
    if(ev.xbutton.subwindow != None)
    {
      XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
      start = ev.xbutton;
    }
    break;
  case ButtonRelease:
    start.subwindow = None;
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
    }
    break;
  }
}

