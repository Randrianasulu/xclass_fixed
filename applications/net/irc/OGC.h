#ifndef __OGC_H
#define __OGC_H

#include <X11/Xlib.h>
#include <xclass/OXClient.h>
#include <xclass/OString.h>

class OGC {
public:
  OGC(OXClient *c, char *fgcolor, char *bgcolor = NULL);
  OGC(OXClient *c, char *fgcolor, unsigned long bgcolor);
  ~OGC();

  OGC *_next;
  GC _gc;
  XFontStruct *_font;
  unsigned long _foreground, _background;

  const char *GetColor() { return(_color->GetString()); }

protected:
  void _MakeGC(unsigned long fgcolor, unsigned long bgcolor);

  OString *_color;
  OXClient *_client;
};

#endif
