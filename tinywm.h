#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>
#include <vector>

#define MOUSE_FOCUS 1

Display * dpy;
Window w = None, zoom = None, focus = None;
XWindowAttributes attr;
XButtonEvent start{0};
unsigned short rect[4]={0};
bool close;

void ListWindow(std::vector<Window> &l)
{
  unsigned int cnt = 0;
  Window r, p, *wlist;
  Window root = RootWindow(dpy, DefaultScreen(dpy));
  XWindowAttributes attr;

  XQueryTree(dpy, root, &r, &p, &wlist, &cnt);
  for(unsigned int i = 0; i < cnt; ++i)
  {
   XGetWindowAttributes(dpy, wlist[i], &attr);
   if(attr.map_state == IsViewable)
     l.push_back(wlist[i]);
  }
}

void SetInput(Display *dpy)
{
  KeyCode CK, EK, FK, HK, RK, TK, XK, ZK;
  CK = XKeysymToKeycode(dpy, XStringToKeysym("c"));
  EK = XKeysymToKeycode(dpy, XK_Escape);
  FK = XKeysymToKeycode(dpy, XStringToKeysym("f"));
  HK = XKeysymToKeycode(dpy, XStringToKeysym("h"));
  RK = XKeysymToKeycode(dpy, XStringToKeysym("r"));
  TK = XKeysymToKeycode(dpy, XK_Tab);
  XK = XKeysymToKeycode(dpy, XStringToKeysym("x"));
  ZK = XKeysymToKeycode(dpy, XStringToKeysym("z"));

  XGrabKey(dpy, CK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, EK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, FK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, HK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, RK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, TK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, XK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, ZK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
  XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
}

inline int ProcessKey(const XEvent &ev)
{
  //printf("keycode: %d\n", ev.xkey.keycode);
  switch(ev.xkey.keycode)
  {
  //alt + tab : 8 + 23
  case 23:
    {
      std::vector<Window> list;;
      ListWindow(list);

      for(Window w : list)
      if(focus != w)
      {
	focus = w;  
        XRaiseWindow(dpy, focus);
	XSetInputFocus(dpy, focus, RevertToParent, CurrentTime);
        break;
      }
    }
    break;
  //alt + r : 8 + 27
  case 27:
    system("dmenu_run &");
    break;
  //alt + f : 8 + 41
  case 41:
    if(ev.xkey.subwindow != None)
      XRaiseWindow(dpy, ev.xkey.subwindow); 
    break;
  //alt + h : 8 + 43
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
  //alt + z : 8 + 52
  case 52:
    if(ev.xkey.subwindow != None && zoom == None)
    {
      zoom = ev.xkey.subwindow;
      XGetWindowAttributes(dpy, zoom, &attr);
      rect[0]=attr.x;
      rect[1]=attr.y;
      rect[2]=attr.width;
      rect[3]=attr.height;
      XMoveResizeWindow(dpy,zoom,0,0,1920,1080);
    }
    else
    if(zoom != None)
    {
      XMoveResizeWindow(dpy,zoom,rect[0],rect[1],rect[2],rect[3]);
      zoom = None;
    }
    break;
  //alt + x : 8 + 53
  case 53:
    if(ev.xkey.subwindow != None && close == 0)
    {
      close = 1;
      XKillClient(dpy, ev.xkey.subwindow);
    }
    break;
  //alt + esc : 8 + 9
  case 9:
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
      #ifdef MOUSE_FOCUS
      focus = ev.xbutton.subwindow; 
      XRaiseWindow(dpy, focus);
      XSetInputFocus(dpy, focus, RevertToParent, CurrentTime);
      #endif

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

