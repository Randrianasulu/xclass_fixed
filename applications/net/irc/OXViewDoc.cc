#include <stdio.h>
#include <xclass/utils.h>
#include <xclass/OResourcePool.h>

#include "OXViewDoc.h"
#include "OXPreferences.h"

#define ALWAYS_SCROLLED 1

extern OSettings  *foxircSettings;
extern unsigned long IRCcolors[];

//----------------------------------------------------------------------

OXViewDocFrame::OXViewDocFrame(OXWindow *parent, int x, int y,
                         unsigned int options, unsigned long back) :
  OXFrame(parent, x, y, options, back) {

  const OPicture *bckgnd_pix = NULL;

  XGCValues gcv;
  int mask;

//  bckgnd_pix = GetResourcePool()->GetDocumentBckgndPicture();
  if (bckgnd_pix) {
//    FatalError("Missing background pixmap: back2.xpm");
    mask = GCTile | GCFillStyle | GCGraphicsExposures;
    SetBackgroundPixmap(bckgnd_pix->GetPicture());
    gcv.tile = bckgnd_pix->GetPicture();
    gcv.fill_style = FillTiled;
    gcv.graphics_exposures = True;
  } else{
    printf("No Pix defaulting..\n");
    mask = GCForeground | GCBackground;
 // gcv.fill_style = FillSolid; | GCFillStyle
//  gcv.line_style = LineSolid; GCLineStyle |
//  gcv.line_width = 0; GCLineWidth  | 
    gcv.foreground = IRCcolors[foxircSettings->GetColorID(20)];
    gcv.background = _defaultDocumentForeground;
    SetBackgroundColor(gcv.foreground);
  }
  _backgc = XCreateGC(GetDisplay(), _id, mask, &gcv);

  _tx = 0;
  _ty = 0;
}

//---------------------------------------------------------------------------

void OXViewDocFrame::SetupBackgroundPic(const char *name) {
  const OPicture *bckgnd_pix = _client->GetPicture(name);
  if (!bckgnd_pix) return;  // set tile to None...

  XGCValues gcv;
  int mask = GCTile | GCFillStyle | GCGraphicsExposures;

  SetBackgroundPixmap(bckgnd_pix->GetPicture());
  gcv.tile = bckgnd_pix->GetPicture();
  gcv.fill_style = FillTiled;
  gcv.graphics_exposures = True;
  XChangeGC(GetDisplay(), _backgc, mask, &gcv);
}

void OXViewDocFrame::SetupBackground(unsigned long back) {

  XGCValues gcv;
  int mask;

  mask = GCForeground | GCBackground;
 // gcv.fill_style = FillSolid; | GCFillStyle
//  gcv.line_style = LineSolid; GCLineStyle |
//  gcv.line_width = 0; GCLineWidth  | 
  gcv.foreground = back;
  gcv.background = _defaultDocumentForeground;
  SetBackgroundColor(back);
  XChangeGC(GetDisplay(), _backgc, mask, &gcv);
}

OXViewDocFrame::~OXViewDocFrame() {
  Clear();
}

void OXViewDocFrame::Clear() {
  _document->Clear();
}

void OXViewDocFrame::Layout() {
  _document->Layout();
}

void OXViewDocFrame::DrawRegion(int x, int y, int w, int h, int clear) {
  XRectangle rect;

  rect.x = x;
  rect.y = y;
  rect.width = w;
  rect.height = h;

  if (clear) {
    int mask = GCTileStipXOrigin | GCTileStipYOrigin;
    XGCValues gcv;

    gcv.ts_x_origin = -_tx;
    gcv.ts_y_origin = -_ty;
    XChangeGC(GetDisplay(), _backgc, mask, &gcv);

    XFillRectangle(GetDisplay(), _id, _backgc, x, y, w, h);
//    XClearArea(GetDisplay(), _id, x, y, w, h, false);

  }

//    XClearArea(GetDisplay(), _id, x, y, w, h,false);
  _document->DrawRegion(GetDisplay(), _id, -_tx, -_ty, &rect);
//  _document->DrawRegion(GetDisplay(), _id, 0, 0, &rect);
}

//---------------------------------------------------------------------------

void OXViewDocFrame::HScrollWindow(int x) {
  XPoint points[4];
  int xsrc, ysrc, xdest, ydest;

  // These points are the same for both cases, so set them here.

  if (x == _tx)
    return;

  points[0].y = points[3].y = 0;
  points[1].y = points[2].y = _h;
  ysrc = ydest = 0;

  if (x < _tx) {
    // scroll left...
    xsrc = 0;
    xdest = _tx - x;
    // limit the destination to the window width
    if (xdest > _w) xdest = _w;
    // Fill in the points array with the bounding box of the area that
    // needs to be redrawn - that is, the area that is not copied.
    points[1].x = points[0].x = 0;
    points[3].x = points[2].x = xdest; // -1;
  } else {
    // scroll right...
    xdest = 0;
    xsrc = x - _tx;
    // limit the source to the window height
    if (xsrc > _w) xsrc = _w;
    // Fill in the points array with the bounding box of the area that
    // needs to be redrawn - that is, the area that is not copied.
    points[1].x = points[0].x = _w - xsrc; // +1;
    points[3].x = points[2].x = _w;
  }
  // Copy the scrolled region to its new position
  XCopyArea(GetDisplay(), _id, _id, _backgc, xsrc, ysrc, _w, _h, xdest, ydest);
  // Set the new origin
  _tx = x;
  DrawRegion(points[0].x, points[0].y,
             points[2].x - points[0].x, points[2].y - points[0].y, True);
}

void OXViewDocFrame::VScrollWindow(int y) {
  XPoint points[4];
  int xsrc, ysrc, xdest, ydest;

  // These points are the same for both cases, so set them here.

  if (y == _ty)
    return;

  points[0].x = points[3].x = 0;
  points[1].x = points[2].x = _w;
  xsrc = xdest = 0;

  if (y < _ty) {
    // scroll down...
    ysrc = 0;
    ydest = _ty - y;
    // limit the destination to the window height
    if (ydest > _h) ydest = _h;
    // Fill in the points array with the bounding box of the area that
    // needs to be redrawn - that is, the area that is not copied.
    points[1].y = points[0].y = 0;
    points[3].y = points[2].y = ydest; // -1;
  } else {
    // scroll up...
    ydest = 0;
    ysrc = y - _ty;
    // limit the source to the window height
    if (ysrc > _h) ysrc = _h;
    // Fill in the points array with the bounding box of the area that
    // needs to be redrawn - that is, the area that is not copied.
    points[1].y = points[0].y = _h - ysrc; // +1;
    points[3].y = points[2].y = _h;
  }
  // Copy the scrolled region to its new position
  XCopyArea(GetDisplay(), _id, _id, _backgc, xsrc, ysrc, _w, _h, xdest, ydest);
  // Set the new origin
  _ty = y;
  DrawRegion(points[0].x, points[0].y,
             points[2].x - points[0].x, points[2].y - points[0].y, True);

{
  XEvent event;

  XSync(GetDisplay(), False);
  while (XCheckTypedWindowEvent(GetDisplay(), _id,
                                GraphicsExpose, &event)) {
    HandleGraphicsExpose((XGraphicsExposeEvent *) &event);
    if (event.xgraphicsexpose.count == 0) break;
  }
}
}

//============================================================================

OXViewDoc::OXViewDoc(const OXWindow *parent, OViewDoc *d, int x, int y,
                       unsigned int options, unsigned long back) :
  OXCompositeFrame(parent, x, y, options,back) {

  SetLayoutManager(new OHorizontalLayout(this));

  _background = back;

  _document = d;
  _document->CreateCanvas(this);
  _canvas = _document->GetCanvas();

  _vscrollbar = new OXVScrollBar(this, 16, 16, CHILD_FRAME);
  _hscrollbar = new OXHScrollBar(this, 16, 16, CHILD_FRAME);

  AddFrame(_canvas, NULL);
  AddFrame(_hscrollbar, NULL);
  AddFrame(_vscrollbar, NULL);

  MapSubwindows();
}

OXViewDoc::~OXViewDoc() {
}

int OXViewDoc::LoadFile(char *fname) {
  char wintitle[100];
  FILE * file = fopen(fname, "r");
  if (file) {
    _document->LoadFile(file);
    fclose(file);
    Layout();
    return True;
  }
  return False;
}

void OXViewDoc::Clear() {
  _canvas->Clear();
  Layout();
}

int OXViewDoc::ProcessMessage(OMessage *msg) {
  OScrollBarMessage *sbmsg = (OScrollBarMessage *) msg;

  switch(msg->type) {
    case MSG_HSCROLL:
      switch(msg->action) {
        case MSG_SLIDERTRACK:
        case MSG_SLIDERPOS:
          _canvas->HScrollWindow(sbmsg->pos);
          break;
      }
      break;
    case MSG_VSCROLL:
      switch(msg->action) {
        case MSG_SLIDERTRACK:
        case MSG_SLIDERPOS:
          _canvas->VScrollWindow(sbmsg->pos);
          break;
      }
      break;
  }
  return True;
}

void OXViewDoc::AdjustScrollbars() {
  int tdw, tdh, tcw, tch;

  tch = _h - (_bw << 1);
  tcw = _w - (_bw << 1);
  tdw = _canvas->_document->GetWidth();
  tdh = _canvas->_document->GetHeight();

  if (_vscrollbar->IsMapped()) {
    tcw -= _vscrollbar->GetDefaultWidth();
    if (_hscrollbar->IsMapped()) {
      tch -= _hscrollbar->GetDefaultHeight();
      if (tcw > tdw) tdw = tcw;
      _hscrollbar->SetRange(tdw, tcw);
    }
    if (tch > tdh) tdh = tch;
    _vscrollbar->SetRange(tdh, tch);
  } else {
    if (_hscrollbar->IsMapped()) {
      if (tcw > tdw) tdw = tcw;
      _hscrollbar->SetRange(tdw, tcw);
    }
  }
}

void OXViewDoc::Layout() {
  int tcw, tch;

  _canvas->_h = (tch = _h - (_bw << 1)) - _hscrollbar->GetDefaultHeight();
  _canvas->_w = (tcw = _w - (_bw << 1)) - _vscrollbar->GetDefaultWidth();
  _canvas->Layout();
  
  if (((tch >= _canvas->_document->GetHeight()) ||
       (_scroll_options & NO_VSCROLL)) &&
      (!(_scroll_options & FORCE_VSCROLL))) {
    _vscrollbar->UnmapWindow();
    _canvas->_ty = 0;
    if (((tcw >= _canvas->_document->GetWidth()) ||
         (_scroll_options & NO_HSCROLL)) &&
        (!(_scroll_options & FORCE_HSCROLL))) {
      _hscrollbar->UnmapWindow();
      _canvas->_tx = 0;
    } else {
      tch -= _hscrollbar->GetDefaultHeight();
      _hscrollbar->MoveResize(_bw, _bw + tch,
                              tcw, _hscrollbar->GetDefaultHeight());
      _hscrollbar->MapWindow();
    }
  } else {
    tcw -= _vscrollbar->GetDefaultWidth();
    if (((tcw >= _canvas->_document->GetWidth()) ||
         (_scroll_options & NO_HSCROLL)) &&
        (!(_scroll_options & FORCE_HSCROLL))) {
      _hscrollbar->UnmapWindow();
      _canvas->_tx = 0;
    } else {
      tch -= _hscrollbar->GetDefaultHeight();
      _hscrollbar->MoveResize(_bw, _bw + tch,
                              tcw, _hscrollbar->GetDefaultHeight());
      _hscrollbar->MapWindow();
    }
    _vscrollbar->MoveResize(_bw + tcw, _bw,
                            _vscrollbar->GetDefaultHeight(), tch);
    _vscrollbar->MapWindow();
  }
  AdjustScrollbars();
  _canvas->MoveResize(_bw, _bw, tcw, tch);
}

void OXViewDoc::ScrollUp() {
  int pos = _canvas->_document->GetHeight() - _canvas->GetHeight();

  if (pos < 0) pos = 0;
  if (pos == _canvas->_ty)
    _canvas->DrawRegion(0, 0, _canvas->_w, _canvas->_h, False);
  _vscrollbar->SetPosition(pos);
}
