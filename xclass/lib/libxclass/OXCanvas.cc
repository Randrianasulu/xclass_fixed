/**************************************************************************

    This file is part of Xclass95, a Win95-looking GUI toolkit.
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

#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXWidget.h>
#include <xclass/OXCanvas.h>


//-----------------------------------------------------------------

OXCanvas::OXCanvas(const OXWindow *p, int w, int h,
           unsigned int options, unsigned long back) :
    OXFrame(p, w, h, options, back) {
    
  _vport = new OXViewPort(this, w-4, h-4, CHILD_FRAME | OWN_BKGND,
                                _whitePixel);
  _hscrollbar = new OXHScrollBar(this, w-4, SB_WIDTH);
  _vscrollbar = new OXVScrollBar(this, SB_WIDTH, h-4);

  _scrolling = CANVAS_SCROLL_BOTH;

  _hscrollbar->Associate(this);
  _vscrollbar->Associate(this);
}
           
OXCanvas::~OXCanvas() {
  // =!= we have to delete these explicitely here, since OXCanvas is
  // not a derivated of OXCompositeFrame, and thus we do not AddFrame()'s
  delete _hscrollbar;
  delete _vscrollbar;
  delete _vport;
}

void OXCanvas::MapSubwindows() {
  OXWindow::MapSubwindows();
  _hscrollbar->MapSubwindows();
  _vscrollbar->MapSubwindows();
  _vport->MapSubwindows();
}

void OXCanvas::AddFrame(OXFrame *f, OLayoutHints *l) {
  ((OXCompositeFrame *)(_vport->GetContainer()))->AddFrame(f, l);
}

void OXCanvas::Layout() {
  OXFrame *_container;
  int need_vsb, need_hsb, cw, ch, tcw, tch;

  need_vsb = need_hsb = False;

  _container = (OXFrame *) _vport->GetContainer();    

  // test whether we need scrollbars

  cw = _w-(_bw << 1);
  ch = _h-(_bw << 1);

  _container->SetWidth(cw);
  _container->SetHeight(ch);

  if (_container->GetDefaultWidth() > cw) {
    if (_scrolling & CANVAS_SCROLL_HORIZONTAL) {
      need_hsb = True;
      ch = _h-(_bw << 1)-_vscrollbar->GetDefaultWidth(); 
      _container->SetHeight(ch);
    }
  }

  if (_container->GetDefaultHeight() > ch) {
    if (_scrolling & CANVAS_SCROLL_VERTICAL) {
      need_vsb = True;
      cw = _w-(_bw << 1)-_hscrollbar->GetDefaultHeight();
      _container->SetWidth(cw);
    }
  }

  // re-check again (putting the scrollbar
  // could have changed things)

  if (_container->GetDefaultWidth() > cw) {
    if (!need_hsb) {
      if (_scrolling & CANVAS_SCROLL_HORIZONTAL) {
        need_hsb = True;
        ch = _h-(_bw << 1)-_vscrollbar->GetDefaultWidth();
        _container->SetHeight(ch);
      }
    }
  }

  if (need_hsb) {
    _hscrollbar->MoveResize(_bw, ch+_bw, cw, _hscrollbar->GetDefaultHeight());
    _hscrollbar->MapWindow();
  } else {
    _hscrollbar->UnmapWindow();
    _hscrollbar->SetPosition(0);
  }

  if (need_vsb) {
    _vscrollbar->MoveResize(cw+_bw, _bw, _vscrollbar->GetDefaultWidth(), ch);
    _vscrollbar->MapWindow();
  } else {
    _vscrollbar->UnmapWindow();
    _vscrollbar->SetPosition(0);
  }

  _vport->MoveResize(_bw, _bw, cw, ch);

  tcw = max(_container->GetDefaultWidth(), cw);
  tch = max(_container->GetDefaultHeight(), ch);
  _container->SetHeight(0); // force a resize in OXFrame::Resize
  _container->Resize(tcw, tch);

  _hscrollbar->SetRange(_container->GetWidth(), _vport->GetWidth());
  _vscrollbar->SetRange(_container->GetHeight(), _vport->GetHeight());
}

int OXCanvas::ProcessMessage(OMessage *msg) {
  OScrollBarMessage *sbmsg;

  switch (msg->type) {
    case MSG_HSCROLL:
      switch (msg->action) {
        case MSG_SLIDERTRACK:
        case MSG_SLIDERPOS:
          sbmsg = (OScrollBarMessage *) msg;
          _vport->SetHPos(-sbmsg->pos);
          break;
      }
      break;

    case MSG_VSCROLL:
      switch (msg->action) {
        case MSG_SLIDERTRACK:
        case MSG_SLIDERPOS:
          sbmsg = (OScrollBarMessage *) msg;
          _vport->SetVPos(-sbmsg->pos);
          break;
      }
      break;

    default:
      break;
   
  }

  return True;
}

void OXCanvas::SetScrolling(int scrolling) {
  if (scrolling != _scrolling) {
    _scrolling = scrolling;
    Layout();
  }
}

OXFrame *OXCanvas::GetFrameFromPoint(int x, int y) {

  if (!Contains(x, y)) return NULL;

  if (_hscrollbar->Contains(x - _hscrollbar->GetX(),
                            y - _hscrollbar->GetY())) return _hscrollbar;

  if (_vscrollbar->Contains(x - _vscrollbar->GetX(),
                            y - _vscrollbar->GetY())) return _hscrollbar;

  OXFrame *f = _vport->GetFrameFromPoint(x - _vport->GetX(),
                                         y - _vport->GetY());
  if (f) return f;

  return this;
}


//-----------------------------------------------------------------

OXViewPort::OXViewPort(const OXWindow *p, int w, int h,
           unsigned int options, unsigned long back) :   
    OXCompositeFrame(p, w, h, options, back) {

    _container = NULL;
    _x0 = _y0 = 0;
    MapSubwindows();
}
