/**************************************************************************

    This file is part of xclass, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
   
#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXTransientFrame.h>


//----------------------------------------------------------------

OXTransientFrame::OXTransientFrame(const OXWindow *p, const OXWindow *main,
                                   int w, int h, unsigned long options) :
  OXMainFrame(p, w, h, options) {

  _main = (OXMainFrame*)main;
  if (main) {
    XSetTransientForHint(GetDisplay(), _id, main->GetId());
    ((OXMainFrame*)main)->RegisterTransient(this);
  }
}

OXTransientFrame::~OXTransientFrame() {
  if (_main) ((OXMainFrame*)_main)->UnregisterTransient(this);
}

// Override this to intercept close...

void OXTransientFrame::CloseWindow() {
  // The following delete calls OXMainFrame's destructor,
  // which does a DestroyWindow()
  delete this;
}
