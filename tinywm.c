// tinywm, 8l, public domain, 2016.
#include "tinywm.h"

int main(void)
{
  XEvent ev;
  struct timeval tv;
  fd_set set;
  dpy = XOpenDisplay(0x0);
  SetInput(dpy);
  int fd = XConnectionNumber(dpy);
  for(;true;)
  {
    tv.tv_sec=0; tv.tv_usec=400000;
    FD_ZERO(&set); FD_SET(fd, &set);
    select(fd+1,&set,0,0,&tv);
    while(XPending(dpy)) XNextEvent(dpy,&ev);
    switch(ev.type)
    {
    case KeyPress:ProcessKey(ev);break;
    case KeyRelease:close = 0;break;
    default: ProcessMouse(ev);break;
    }
  }
  return 0;
}

