/**************************************************************************

    This file is part of a xclass desktop manager.
    Copyright (C) 1996-2000 David Barth, Hector Peraza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#include <X11/X.h>
#include <X11/Xlib.h>

#include "OXDesktopRoot.h"


int OXDesktopRoot::_errorCode;


//----------------------------------------------------------------------

OXDesktopRoot::OXDesktopRoot(OXClient *c, int *retc) :
  OXRootWindow(c, c->GetRoot()->GetId()) {

    _errorCode = None;

    XSync(GetDisplay(), False);
    XErrorHandler oldHandler = XSetErrorHandler(OXDesktopRoot::ErrorHandler);

    XGrabButton(GetDisplay(), AnyButton, AnyModifier, _id,
                True, ButtonPressMask | ButtonReleaseMask, 
                GrabModeSync, GrabModeAsync, None, None);

    XSync(GetDisplay(), False);
    XSetErrorHandler(oldHandler);

    *retc = _errorCode;
}

OXDesktopRoot::~OXDesktopRoot() {
  XUngrabButton(GetDisplay(), AnyButton, AnyModifier, _id);
}

int OXDesktopRoot::ErrorHandler(Display *dpy, XErrorEvent *event) {
  _errorCode = event->error_code;
  return 0;
}

int OXDesktopRoot::HandleEvent(XEvent *event) {

  switch (event->xany.type) {
    case ButtonPress:
    case ButtonRelease:
      HandleButton((XButtonEvent *) event);
      break;

    case MotionNotify:
      HandleMotion((XMotionEvent *) event);
      break;

    default:
      return OXRootWindow::HandleEvent(event);
      break;
  }

  return True;
}

int OXDesktopRoot::HandleButton(XButtonEvent *event) {

  if (event->subwindow != None) {

    // pass click event to the window or WM
    XSync(GetDisplay(), False);
    XAllowEvents(GetDisplay(), ReplayPointer, CurrentTime);
    XSync(GetDisplay(), False);

  } else {

    // process click, but do not pass it to the WM
    XSync(GetDisplay(), False);
    XAllowEvents(GetDisplay(), AsyncPointer, CurrentTime);
    XSync(GetDisplay(), False);

    ODesktopRootMessage msg(MSG_DESKTOPROOT, MSG_CLICK, event->button,
                            event->x_root, event->y_root);
    SendMessage(_msgObject, &msg);
  }

  return True;
}

int OXDesktopRoot::HandleMotion(XMotionEvent *event) {
  return True;
}

