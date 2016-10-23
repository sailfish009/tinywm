// tinywm, 8l, zlib license, 2016
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>
#include <vector>

#define SCREEN_WIDTH   1920
#define SCREEN_HEIGHT  1080

Display * dpy;
Window w = None, zoom = None, focus = None;
XWindowAttributes attr;
XButtonEvent start{0};
unsigned short rect[4]={0};
bool close;
bool list_toggle;

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
  const KeyCode AK = XKeysymToKeycode(dpy, XStringToKeysym("a"));
  const KeyCode CK = XKeysymToKeycode(dpy, XStringToKeysym("c"));
  const KeyCode EK = XKeysymToKeycode(dpy, XK_Escape);
  const KeyCode FK = XKeysymToKeycode(dpy, XStringToKeysym("f"));
  const KeyCode HK = XKeysymToKeycode(dpy, XStringToKeysym("h"));
  const KeyCode RK = XKeysymToKeycode(dpy, XStringToKeysym("r"));
  const KeyCode TK = XKeysymToKeycode(dpy, XK_Tab);
  const KeyCode XK = XKeysymToKeycode(dpy, XStringToKeysym("x"));
  const KeyCode ZK = XKeysymToKeycode(dpy, XStringToKeysym("z"));
  XGrabKey(dpy, AK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, CK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, EK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, FK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, HK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, RK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, TK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, XK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabKey(dpy, ZK, Mod1Mask,DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
  XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, 
    None, None);
  XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True,
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, 
    None, None);
}

inline void ProcessKey(const XEvent &ev)
{
  switch(ev.xkey.keycode)
  {
  case 23:
    {
      std::vector<Window> list;
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
  case 27:
    system("dmenu_run &");
    break;
  case 38:
    {
      std::vector<Window> list;
      ListWindow(list);
      switch(list.size())
      case 2:
        if(list_toggle)
	{
	  list_toggle = false;
          XMoveResizeWindow(dpy,list[1],0,0,SCREEN_WIDTH/2-1,SCREEN_HEIGHT);
          XMoveResizeWindow(dpy,list[0],SCREEN_WIDTH/2,0,SCREEN_WIDTH/2,SCREEN_HEIGHT);
	}
	else
	{
	  list_toggle = true;
          XMoveResizeWindow(dpy,list[0],0,0,SCREEN_WIDTH/2-1,SCREEN_HEIGHT);
          XMoveResizeWindow(dpy,list[1],SCREEN_WIDTH/2,0,SCREEN_WIDTH/2,SCREEN_HEIGHT);
	}
        break;
    }
    break;
  case 41:
    if(ev.xkey.subwindow != None)
      XRaiseWindow(dpy, ev.xkey.subwindow); 
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
    if(ev.xkey.subwindow != None && close == 0)
    {
      close = 1;
      XKillClient(dpy, ev.xkey.subwindow);
    }
    break;
  case 54:
    //todo: create little window
    break;
  case 9:
    exit(0);
  }
}

inline void ProcessMouse(const XEvent &ev)
{
  switch(ev.type)
  {
  case ButtonPress:
    if(ev.xbutton.subwindow != None)
    {
      focus = ev.xbutton.subwindow; 
      XRaiseWindow(dpy, focus);
      XSetInputFocus(dpy, focus, RevertToParent, CurrentTime);
      XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
      start = ev.xbutton;
    }
    break;
  case ButtonRelease:
    {
      start.subwindow = None;
      if(ev.xbutton.x_root > SCREEN_WIDTH - 10)
        XMoveResizeWindow(dpy,ev.xbutton.subwindow,SCREEN_WIDTH*2/3,0,SCREEN_WIDTH*1/3,SCREEN_HEIGHT);
      if(ev.xbutton.x_root < 10)
        XMoveResizeWindow(dpy,ev.xbutton.subwindow,0,0,SCREEN_WIDTH*2/3,SCREEN_HEIGHT);
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
    }
    break;
  }
}

