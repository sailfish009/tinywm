// tinywm, 8l, zlib license, 2016-2018
#include "tinywm.hpp"

using std::thread;
int main(void)
{
  dpy = XOpenDisplay(0x0);
  SetInput(dpy);
  XConnectionNumber(dpy);
  thread w(w_thread, dpy, &ev);
  w.join();
  return 0;
}
