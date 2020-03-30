// tinywm, 8l, zlib license, 2016-2018
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

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
int win_count = 0;

void ListWindow(std::vector<Window> &l)
{
  unsigned int cnt = 0;
  Window r, p, *wlist;
  XWindowAttributes attr;
  XQueryTree(dpy, root, &r, &p, &wlist, &cnt);
  for(unsigned int i = 0; i < cnt; ++i)
  {
    XGetWindowAttributes(dpy, wlist[i], &attr);
    if(attr.map_state == IsViewable)
      l.push_back(wlist[i]);
  }
}

void run(const char* cmd)
{
  system(cmd);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ++win_count;
  std::vector<Window> wlist;
  ListWindow(wlist);
  std::string win_name = std::to_string(win_count);
  for(Window w : wlist)
  {
    char *name;
    XFetchName(dpy, w, &name);
    if(isdigit(name[0])==0)
    {
      XStoreName(dpy, w, win_name.c_str());
      break;
    }
  }
}

void debug()
{
  std::vector<Window> wlist;
  ListWindow(wlist);
  for(Window w : wlist)
  {
    char *name;
    XFetchName(dpy, w, &name);
    // print("name: %s\n",name)
  }
}

void SetInput(Display *dpy)
{
  root = RootWindow(dpy, DefaultScreen(dpy));
  SET_KEY(dpy, "a");  SET_KEY(dpy, "c");  SET_KEY(dpy, "f");  SET_KEY(dpy, "h");  
  SET_KEY(dpy, "r");  SET_KEY(dpy, "t");  SET_KEY(dpy, "x");  SET_KEY(dpy, "z");
  SET_KEY2(dpy, XK_Escape); SET_KEY2(dpy, XK_Tab); SET_KEY3(dpy, 1, Mod1Mask); 
  SET_KEY3(dpy, 3, Mod1Mask); SET_KEY3(dpy, 8, 0); SET_KEY3(dpy, 9, 0);
  XSetWindowBackground(dpy, root, 0x0192c6);
  XClearWindow(dpy, root);

  run("xclock &");
  run("term &");
  run("win &");
  system("sct 2300");
}

// inline void ProcessWin(const XEvent &ev)
// {
//   if( ev.type == ButtonPress && ev.xbutton.subwindow != None)
//   {
//     int x_pos = ev.xbutton.x_root;
//     int y_pos = ev.xbutton.y_root;
//     XWindowAttributes xwa;
//     XGetWindowAttributes(dpy, ev.xbutton.subwindow, &xwa);
//     if( 
//         (x_pos > xwa.x && x_pos < (xwa.x + xwa.width)) && 
//         (y_pos > xwa.y && y_pos < (xwa.y + xwa.height))
//       )
//     {
//       focus = ev.xbutton.subwindow; 
//       XRaiseWindow(dpy, focus);
//       XSetInputFocus(dpy, focus, RevertToParent, CurrentTime);
//       XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
//       start = ev.xbutton;
//     }
//   }
// }

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
      auto lambda =[](Display *d, const XEvent &e, Window &f, XWindowAttributes &a)
      {
        f = e.xbutton.subwindow; 
        XRaiseWindow(d, f);
        XSetInputFocus(d, f, RevertToParent, CurrentTime);
        XGetWindowAttributes(d, e.xbutton.subwindow, &a);
      };
      switch(ev.xbutton.button)
      {
      //page up
      case 8:
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Up), 1, CurrentTime);
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Up), 0, CurrentTime);
      break;

      //page down
      case 9:
      // to focus window without use window title bar
      if(focus != ev.xbutton.subwindow) lambda(dpy, ev, focus, attr);
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Down), 1, CurrentTime);
      XTestFakeKeyEvent(dpy,XKeysymToKeycode(dpy, XK_Page_Down), 0, CurrentTime);
      break;

      default:
      lambda(dpy, ev, focus, attr);
      // focus = ev.xbutton.subwindow; 
      // XRaiseWindow(dpy, focus);
      // XSetInputFocus(dpy, focus, RevertToParent, CurrentTime);
      // XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
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

void *w_thread(Display* dpy, XEvent *ev)
{
  for(;;)
  {
    while(XCheckMaskEvent(dpy, 0xFFFFFFFF, ev))
    {
      switch(ev->type)
      {
      case KeyPress:ProcessKey(*ev);break;
      case KeyRelease:b_close = 0;break;
      default: ProcessMouse(*ev);break;
      }
    }
    std::this_thread::sleep_for(std::chrono::nanoseconds(1000000));
    // TODO semaphore and keyboard signal
  }
}

