#ifndef __APPEARANCE_TAB_H
#define __APPEARANCE_TAB_H

#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXCompositeFrame.h>
#include <xclass/OXColorSelect.h>
#include <xclass/OXLabel.h>
#include <xclass/OXTextButton.h>
#include <xclass/OXDDListBox.h>
#include <xclass/OString.h>
#include <xclass/utils.h>


class OXGC;
class OXFont;

struct Sconfig {  // not exactly the same as in OResourcePool
public:
  char default_font[256];
  char menu_font[256];
  char menu_hi_font[256];
  char doc_fixed_font[256];
  char doc_prop_font[256];
  char icon_font[256];
  char status_font[256];

  char frame_bg_color[256];
  char frame_fg_color[256];
  char doc_bg_color[256];
  char doc_fg_color[256];
  char sel_bg_color[256];
  char sel_fg_color[256];
  char tip_bg_color[256];
  char tip_fg_color[256];
  char desktop_bg_color[256];

  char frame_bg_pixmap[256];
  char doc_bg_pixmap[256];
};


//----------------------------------------------------------------------

class OXSampleView : public OXFrame {
public:
  OXSampleView(const OXWindow *p);
  ~OXSampleView();
  
  virtual void Resize(int w, int h);
  virtual void MoveResize(int x, int y, int w, int h);

  void ChangeSettings(Sconfig *config);

protected:
  virtual void _DoRedraw();

  void _CreatePixmap();
  void _Draw();
  
  void _DrawWindow(Drawable dst, int x, int y, int w, int h, int active);
  void _DrawButton(Drawable dst, int x, int y, int w, int h, const OPicture *p);

  Pixmap _pix;
  OXGC *_drawGC;
  unsigned long _desktopBgndPixel,
                _frameBgndPixel, _frameFgndPixel,
                _selBgndPixel, _selFgndPixel,
                _docBgndPixel, _docFgndPixel,
                _hilitePixel, _shadowPixel; //_blackPixel;
  const OXFont *_menuFont, *_iconFont, *_statusFont, *_defaultFont,
               *_tipFont, *_docFixedFont, *_docPropFont;
  int _pixw, _pixh;
  int _wmBorderWidth, _wmTitleHeight;
  const OPicture *_tbutton[3];
};

class OXAppearanceTab : public OXCompositeFrame {
public:
  OXAppearanceTab(const OXWindow *tab, const OPicture *mon);
  virtual ~OXAppearanceTab();

  virtual int ProcessMessage(OMessage *msg);

protected:
  int InitServer();
  int SetProperty();
  int SelItem(int ix);
  
  int LoadSchemes();
  int SaveSchemes();

  void PrintSettings();

  char *ColorName(unsigned long color);
  char *ColorName(OColor color);
  char *FontName(const OXFont *font);
  char *PixmapName(const OPicture *pic);

  OXSampleView  *_sample;
  OXDDListBox   *ddlb;
  OLayoutHints  *ly1, *ly2;
  OXColorSelect *fgtr, *bgtr;
  OXTextButton  *fontb, *applyb;

  Atom _XCLASS_RESOURCES;
  char *_currentFont, *_currentFG, *_currentBG;
  Sconfig settings;
  Colormap root_colormap;

  OLayoutHints *CommonCLayout;
};


#endif  // __APPEARANCE_TAB_H
