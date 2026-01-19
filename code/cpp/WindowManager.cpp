#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <stdlib.h>

 /**
  * @brief emergency exit.
  * 
  * @param msg 
  */
void panic(char *msg)
{
  puts(msg);
  exit(EXIT_FAILURE);
}

Display *dpy;
Window root;

int main(void)
{
  dpy = XOpenDisplay(NULL);
  if (dpy == NULL)
  {
    panic("Unable to open a X display.");
  }

  root = DefaultRootWindow(dpy);

  XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);

  XGrabButton(dpy, Button1, 0, root, 0, ButtonPressMask, GrabModeSync, GrabModeAsync, NULL, NULL);

  Cursor cursor = XCreateFontCursor(dpy, XC_left_ptr);
  XDefineCursor(dpy, root, cursor);

  XSync(dpy, 0);

  XEvent e;
  for (;;)
  {
    XNextEvent(dpy, &e);
    switch (e.type)
    {
    case ButtonPress:
      XAllowEvents(dpy, ReplayPointer, CurrentTime);
      XSync(dpy, 0);
      puts("Left button pressed");
      break;
    case Expose:
      puts("exposed");
      break;
    case ClientMessage:
      puts("client message");
      break;
    case MapNotify:
      puts("map notify");
    default:
      puts("Unexpected event.");
      break;
    }
    // Ensures that X will proccess the event.
    XSync(dpy, 0);
  }

  // Close the display
  XCloseDisplay(dpy);
}
