// tinywm, public domain, 2016.
#include "tinywm.h"

int main(void)
{
  XEvent ev;
  dpy = XOpenDisplay(0x0);
  SetInput(dpy);
  for(;true;)
  {
    XNextEvent(dpy, &ev);
    switch(ev.type)
    {
      case KeyPress: switch(ProcessKey(ev)) case 0:return 0; break;
      case KeyRelease: close = 0; break;
      default: ProcessMouse(ev); break;
    }
  }
}

