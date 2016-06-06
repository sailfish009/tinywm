/* TinyWM is written by Nick Welch <nick@incise.org> in 2005 & 2011.
 *
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include "tinywm.h"

int main(void)
{
  XEvent ev;
  bool close = 0;
   
  if(!(dpy = XOpenDisplay(0x0))) 
    return 1;

  int run = 1;

  SetInput(dpy);

  for(;run;)
  {
    XNextEvent(dpy, &ev);
    switch(ev.type)
    {
    case KeyPress:
      run = ProcessKey(ev, &close);
    case KeyRelease:
      close = 0;
      break;
    default:
      ProcessMouse(ev);
      break;
    }
  }
}

