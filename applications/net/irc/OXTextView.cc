#include <stdio.h>

#include "OXTextView.h"

#define ALWAYS_SCROLLED 1

//----------------------------------------------------------------------

OXTextFrame::OXTextFrame(OXWindow *parent, int x, int y,
                         unsigned int options, unsigned long back) :
  OXViewDocFrame(parent, x, y, options, back) {

  _gclist = NULL;
}

OXTextFrame::~OXTextFrame() {
  Clear();
}

OGC *OXTextFrame::GetGC(char *color) {
  OGC *returnval = NULL;

  for(OGC *i = _gclist; i; i = i->_next) {
    if (!strcmp(i->GetColor(), color)) {
      returnval = i;
      break;
    }
  }
  if (!returnval) {
    returnval = new OGC(_client, color, _defaultDocumentBackground);
    returnval->_next = _gclist;
    _gclist = returnval;
  }
  return returnval;
}

void OXTextFrame::Layout() {
  int dw, dh;

  _document->SetWidth(_w);
  dw = _document->GetWidth();
  if (dw < 1) dw = 1;

  dh = _document->GetHeight();
  if (dh < 1) dh = 1;

  OXViewDocFrame::Layout();
}
