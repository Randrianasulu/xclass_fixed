#ifndef __OTEXTDOC_H
#define __OTEXTDOC_H

#include <stdio.h>
#include <X11/Xlib.h>

#include "OLineDoc.h"

#define MAXLINES    50000

class OXTextFrame;
class OXViewDoc;

class OTextDoc : public OViewDoc {
friend class OXTextFrame;

protected:
  OXViewDoc *_main_widget;
  OGC *_gc;

public:
  OTextDoc();
  ~OTextDoc();

  void CreateCanvas(OXViewDoc *p);
  void SetCanvas(OXTextFrame *c);

  int SetWidth(int w) { _w = w; return True; }

  void Clear();
  int AddLine(OLineDoc *line);
  int LoadFile(FILE *file);

  void Layout();

  void DrawRegion(Display *dpy, Drawable d, int x, int y, XRectangle *rect);

  OXTextFrame *_style_server;

protected:

  OLineDoc *_lines;   // lines of text
};

#endif
