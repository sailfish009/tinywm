// tinywm, 8l, zlib license, 2016-2018
#include "tinywm.h"
#include <thread>
#include <chrono>

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

int main(void)
{
  dpy = XOpenDisplay(0x0);
  SetInput(dpy);
  XConnectionNumber(dpy);
  std::thread w(w_thread, dpy, &ev);
  w.join();
  return 0;
}
