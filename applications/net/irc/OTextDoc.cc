#include <unistd.h>

#include "OTextDoc.h"
#include "OXTextView.h"

//----------------------------------------------------------------------

OTextDoc::OTextDoc() {
  _w = _h = 0;
  _lines = NULL;
}

OTextDoc::~OTextDoc() {
  Clear();
}

void OTextDoc::CreateCanvas(OXViewDoc *p) {
  _style_server = new OXTextFrame(p, 1, 1, CHILD_FRAME);
  _style_server->SetDocument(this);
  _canvas = _style_server;
  _main_widget = p;
  _gc = _style_server->GetGC("white");
}

void OTextDoc::SetCanvas(OXTextFrame *c) {
  _style_server = c;
  _canvas = c;
}

void OTextDoc::Clear() {
  _lines = NULL;
  _w = _h = 0;
}

int OTextDoc::AddLine(OLineDoc *line) {
  if (_lines)
    _lines->InsertBefore(line);
  else
    _lines = line;
  line->Layout();
  _h += line->GetHeight();
  _main_widget->AdjustScrollbars();
}

int OTextDoc::LoadFile(FILE *fp) {
  OLineDoc *line;

  if (fp == NULL) return False;

  Clear();

  // Read each line of the file into the buffer.

  line = new OLineDoc();
  line->SetCanvas(_style_server);
  while (line->LoadFile(fp)) {
    AddLine(line);

    line = new OLineDoc();
    line->SetCanvas(_style_server);
  }

  // Remember the number of lines, and initialize the current line
  // number to be 0.

  return True;
}

void OTextDoc::Layout() {
  OLineDoc *i;
  int _tmp_h = _h;

  _h = 0;
  i = _lines;
  if (i) {
    do {
      i->Layout();
      _h += i->GetHeight();
      i = i->next;
    } while (i != _lines);
  }
}

void OTextDoc::DrawRegion(Display *dpy, Drawable d,
                          int x, int y, XRectangle *rect) {
  OLineDoc *i;
  //GC gc = _gc->_gc;
  int lh = _gc->_font->ascent + _gc->_font->descent;
  int yloc = y;

  i = _lines;
  if (i) {
    do {
      if ((yloc - lh <= rect->y + rect->height) &&
          (yloc + i->GetHeight() >= rect->y))
        i->DrawRegion(dpy, d, x, yloc, rect);
      yloc += i->GetHeight();
      i = i->next;
    } while (i != _lines);
  }
}
