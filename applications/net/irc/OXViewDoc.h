#ifndef __OXVIEWDOC_H
#define __OXVIEWDOC_H

#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXCompositeFrame.h>
#include <xclass/OXWidget.h>
#include <xclass/OXScrollBar.h>

#include "OViewDoc.h"

class OXViewDoc;

class OXViewDocFrame : public OXFrame, public OXWidget {
friend class OXViewDoc;

protected:
  OViewDoc *_document;

  GC _backgc;

public:
  OXViewDocFrame(OXWindow *parent, int w, int h, unsigned int options,
              unsigned long back = _defaultDocumentBackground);
  virtual ~OXViewDocFrame();

  void SetDocument(OViewDoc *d) { _document = d; }
  void SetupBackground(unsigned long back);
  void SetupBackgroundPic(const char *name);
  void Clear();
  int  GetDocHeight() { return _document->GetHeight(); }
  int  GetDocWidth() { return _document->GetWidth(); }
  void HScrollWindow(int x);
  void VScrollWindow(int y);

  virtual int HandleExpose(XExposeEvent *event) {
    DrawRegion(event->x, event->y, event->width, event->height, True);
    return True;
  }
  virtual int HandleGraphicsExpose(XGraphicsExposeEvent *event) {
    DrawRegion(event->x, event->y, event->width, event->height, True);
    return True;
  }

  virtual void Layout();

protected:
  virtual void DrawRegion(int x, int y, int w, int h, int clear);

  int _tx, _ty;
};

#define NO_HSCROLL    1
#define NO_VSCROLL    2
#define FORCE_HSCROLL 4
#define FORCE_VSCROLL 8

class OXViewDoc : public OXCompositeFrame {

protected:
  unsigned long _background;

public:
  OXViewDoc(const OXWindow *parent, OViewDoc *d,
            int w, int h, unsigned int options,
            unsigned long back = _defaultDocumentBackground);
  virtual ~OXViewDoc();

  void SetScrollOptions(int o) { _scroll_options = o; }

  int  LoadFile(char *fname);
  void Clear();
  virtual int ProcessMessage(OMessage *msg);
  virtual void AdjustScrollbars();
  virtual void Layout();
  virtual ODimension GetDefaultSize() const { return ODimension(_w, _h); }
  void SetupBackground(unsigned long back) { _canvas->SetupBackground(back); }
  void SetupBackgroundPic(const char *name) { _canvas->SetupBackgroundPic(name); }

  int GetBackground() { return _background; }

  void ScrollUp();

protected:
  OViewDoc *_document;
  OXViewDocFrame  *_canvas;
  OXHScrollBar *_hscrollbar;
  OXVScrollBar *_vscrollbar;

  int _scroll_options;
};

#endif
