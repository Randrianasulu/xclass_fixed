#include "OGC.h"

#include <xclass/OXWindow.h>
#include <xclass/OResourcePool.h>

#include "OXPreferences.h"

extern OSettings *foxircSettings;

//----------------------------------------------------------------------

void OGC::_MakeGC(unsigned long fgcolor, unsigned long bgcolor) {
  XGCValues gcv;
  int mask = GCForeground | GCBackground |
             GCFont | GCFillStyle | GCGraphicsExposures;

//  char *fontname = "-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*";
  const char *fontname = foxircSettings->GetFont();
//  printf("Font is [%s]\n",fontname);
   
  _font = XLoadQueryFont(_client->GetDisplay(),fontname);

  if (!_font) {
    Debug(DBG_WARN, "Could not load font \"%s\", trying \"fixed\"...\n",
          fontname);
    _font = XLoadQueryFont(_client->GetDisplay(), fontname = "fixed");
    if (!_font) FatalError("Could not load font \"fixed\", exiting.");
  }

  gcv.background = bgcolor;
  gcv.foreground = fgcolor;
  gcv.fill_style = FillSolid;
  gcv.graphics_exposures = False;
  gcv.font = _font->fid;

  _gc = XCreateGC(_client->GetDisplay(), _client->GetRoot()->GetId(),
                  mask, &gcv);

  _foreground = gcv.foreground;
  _background = gcv.background;
}

//----------------------------------------------------------------------

OGC::OGC(OXClient *c, char *fgcolor, char *bgcolor) {
  unsigned long backtmp, foretmp;

  _client = c;

  if (bgcolor)
    backtmp = c->GetColorByName(bgcolor);
  else
    backtmp = c->GetResourcePool()->GetDocumentBgndColor();

  foretmp = c->GetColorByName(fgcolor);
  _color = new OString(fgcolor);

  _MakeGC(foretmp, backtmp);
}

OGC::OGC(OXClient *c, char *fgcolor, unsigned long bgcolor) {

  _client = c;

  unsigned long foretmp = c->GetColorByName(fgcolor);
  _color = new OString(fgcolor);

  _MakeGC(foretmp, bgcolor);
}

OGC::~OGC() {
  XFreeGC(_client->GetDisplay(), _gc);
  XFreeFont(_client->GetDisplay(), _font);
}
