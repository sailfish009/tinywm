#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <algorithm>

KeyCode XK;
KeyCode RK;
KeyCode HK;
KeyCode FK;
KeyCode EK;

void SetInput(Display *dpy)
{
  XK = XKeysymToKeycode(dpy, XStringToKeysym("x"));
  RK = XKeysymToKeycode(dpy, XStringToKeysym("r"));
  HK = XKeysymToKeycode(dpy, XStringToKeysym("h"));
  FK = XKeysymToKeycode(dpy, XStringToKeysym("f"));
  EK = XKeysymToKeycode(dpy, XK_Escape);

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

