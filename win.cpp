#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
// #include <ctime>

bool set_focus = false;

void run(int i)
{
  switch(i)
  {
  case 0:
      system("firefox &");
      break;
  case 1:
      system("term &");
      break;
  case 2:
      system("gedit &");
      break;
  case 3:
      system("nautilus &");
      break;
  case 4:
      system("thunderbird &");
      break;
  case 5:
      system("sct &");
      break;
  case 6:
      system("sct 2300 &");
      break;
  case 7:
      break;
  case 8:
      break;
      // focus
  case 9:
      break;
  }
}

// inline const char* current_time()
// {
//   time_t c_time;
//   struct tm local_time;
//   time(&c_time);
//   // localtime_s(&local_time, &c_time);
//   localtime_r(&c_time, &local_time);
//   int Hour = local_time.tm_hour;
//   int Min = local_time.tm_min;
//   char str[5] = {0};
//   snprintf(str, 5, "%02d:%02d", Hour, Min);
//   return str;
// }
 
int main(void) 
{
  Display *d;
  Window w;
  XEvent e;
  const char *w_name[10] =
  {
    "web",
    "cons",
    "edit",
    "file",
    "mail",
    "file",
    "sctn",
    "sctl",
    "hide",
    "focus"
  };
  int s;

  d = XOpenDisplay(NULL);
  if (d == NULL) 
  {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  const int n_program = 10;
  const int text_x_offset = 5;
  const int text_y_offset = 25;
  const int rw = 50;
  const int y_pos = 300;

  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 0, y_pos, 50, 500, 1,
                          BlackPixel(d, s), WhitePixel(d, s));
  // XSelectInput(d, w, ExposureMask | KeyPressMask | ButtonPressMask);
  XSelectInput(d, w, EnterWindowMask | LeaveWindowMask |ExposureMask | KeyPressMask | ButtonPressMask);
  XMapWindow(d, w);

  while (1) 
  {
    XNextEvent(d, &e);
    switch(e.type)
    {
    case Expose:
    {
      for(int i=0; i < n_program; ++i)
      {
        XDrawRectangle(d, w, DefaultGC(d, s), 0, rw*i,  rw, rw);
        if(i != (n_program -1))
          XDrawString(d, w, DefaultGC(d, s), text_x_offset, rw*i + text_y_offset, w_name[i], strlen(w_name[i]));
      }

      // const char* current_t = current_time();
      // XDrawString(d, w, DefaultGC(d, s), text_x_offset, rw*(n_program-1) + text_y_offset, 
      //   current_t, 5);
    }
    break;
    case ButtonPress:
    {
      if (set_focus && e.xbutton.subwindow != None)
      {
        printf("Leave\n");
        Window focus = e.xbutton.subwindow;
        XRaiseWindow(d, focus);
        XSetInputFocus(d, focus, RevertToParent, CurrentTime);
        // XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
        // start = ev.xbutton;
      }
      else
      {
        const int x = e.xmotion.x_root;
        const int y = e.xmotion.y_root;
        if( x > 0 && x < rw)
        {
          for(int i=0; i < n_program; ++i)
          {
            int y_min = rw*i + y_pos;
            int y_max = rw*(i+1) + y_pos;
            if(y > y_min && y < y_max)
            {
              run(i);
              break;
            }
          }
        }
      }
    }
    break;
    case EnterNotify:
    {
      const int x = e.xmotion.x_root;
      const int y = e.xmotion.y_root;
      if( x > 0 && x < rw)
      {
        const int i = 9;
        int y_min = rw*i + y_pos;
        int y_max = rw*(i+1) + y_pos;
        if(y > y_min && y < y_max)
        {
          // printf("Enter\n");
          set_focus = true;
          break;
        }
      }
    }
    break;
    case LeaveNotify:
    {
      printf("Leave\n");
    }
    break;
    case KeyPress:
    break;
    }
  }

  XCloseDisplay(d);
  return 0;
}
