#ifndef __OXTEXTVIEW_H
#define __OXTEXTVIEW_H

#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXCompositeFrame.h>
#include <xclass/OXScrollBar.h>

#include "OGC.h"
#include "OTextDoc.h"
#include "OXViewDoc.h"

class OXTextFrame : public OXViewDocFrame {
protected:
  OGC *_gclist;

public:
  OXTextFrame(OXWindow *parent, int w, int h, unsigned int options,
              unsigned long back = _defaultDocumentBackground);
  virtual ~OXTextFrame();

  OGC *GetGC(char *color);

  void Layout();

protected:
  //void DrawRegion(int x, int y, int w, int h);
};

#endif
