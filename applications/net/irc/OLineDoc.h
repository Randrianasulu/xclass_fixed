#ifndef __OLINEDOC_H
#define __OLINEDOC_H

#include <stdio.h>
#include <X11/Xlib.h>

#include "OViewDoc.h"
#include "OGC.h"


#define MAXLINESIZE       8000
#define MARGIN            3

#define ATTRIB_NORMAL     0x00
#define ATTRIB_BOLD       0x01
#define ATTRIB_REVERSE    0x02
#define ATTRIB_UNDERLINE  0x04
#define ATTRIB_FGCOLOR    0x08
#define ATTRIB_BGCOLOR    0x10


class OGC;
class OXTextFrame;

class OLineDoc : public OViewDoc {
public:
  OLineDoc();
  ~OLineDoc();

  void SetCanvas(OXTextFrame *c);
  void SetColor(char *c);
  void SetColor(int color);

  OLineDoc *next, *prev;
  OLineDoc *InsertBefore(OLineDoc *l);
  OLineDoc *InsertAfter(OLineDoc *l);

  void Clear();
  int  Fill(char *buf);
  int  LoadFile(FILE *file);

  void Layout();
  void DrawRegion(Display *dpy, Drawable d, int x, int y, XRectangle *rect);

  void DrawLine(Display *dpy, Drawable d, int x, int y, int from, int len);

  int  DrawLineSegment(Display *dpy, Drawable d, int x, int y,
                       char *str, int len, char attrib, char color);

protected:
  OXTextFrame *_style_server;

  char *chars;    // line of text
  char *color;    // bg and fg colors
  char *attrib;   // attrib (bold, reverse, etc...)
  int  lnlen;     // length of line (i.e. number of chars)

  OGC  *_gc;
  int  ll;        // length of displayed line in chars
};

#endif  // __OLINEDOC_H
