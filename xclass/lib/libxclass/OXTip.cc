/**************************************************************************

    This file is part of xclass95.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

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

#include <xclass/utils.h>
#include <xclass/ODimension.h>
#include <xclass/OXTip.h>

OXTip::OXTip(const OXWindow *p, OString *text) :
  OXCompositeFrame(p, 10, 10, HORIZONTAL_FRAME | RAISED_FRAME) {

  XSetWindowAttributes attr;
  unsigned long mask;

  mask = CWOverrideRedirect | CWSaveUnder;
  attr.override_redirect = True;
  attr.save_under = True;

  XChangeWindowAttributes(GetDisplay(), _id, mask, &attr);
  SetBackgroundColor(_client->GetColorByName("LightYellow"));

  _label = new OXLabel(this, text);
  _label->SetBackgroundColor(_client->GetColorByName("LightYellow"));

  AddFrame(_label, _ll = new OLayoutHints(LHINTS_LEFT | LHINTS_TOP,
                                          2, 3, 0, 0));
  MapSubwindows();
  Resize(GetDefaultSize());
}

OXTip::~OXTip() {
  delete _ll;
  XDestroyWindow(GetDisplay(), _id);
}

void OXTip::DrawBorder() {
  DrawLine(_shadowGC, 0, 0, _w-2, 0);
  DrawLine(_shadowGC, 0, 0, 0, _h-2);
  DrawLine(_blackGC,  0, _h-1, _w-1, _h-1);
  DrawLine(_blackGC,  _w-1, _h-1, _w-1, 0);
}

void OXTip::SetText(OString *text) {
  _label->SetText(text);
  Resize(GetDefaultSize());
}

void OXTip::Show(int x, int y) {
  Move(x, y);
  MapWindow();
  RaiseWindow();
}

void OXTip::Hide() {
  UnmapWindow();
}
