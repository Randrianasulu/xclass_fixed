#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <xclass/OXSpinner.h>
#include <xclass/OXColorSelect.h>
#include <xclass/OXFontDialog.h>
#include <xclass/OResourcePool.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXFont.h>
#include <xclass/OGC.h>

#include "main.h"
#include "appearance_tab.h"

#include "bclose.xpm"
#include "bmax.xpm"
#include "bmin.xpm"


// TODO:
// - when used with fvwm95, add a "fvwm95" tab to configure fvwm95 settings
// - fix fvwm95/98 bugs regarding window recoloring
// - load/save schemes in some rc file
// - get wm decoration geometry from fvwm95 if in module mode


#define HAS_BG_COLOR        (1<<0)
#define HAS_FG_COLOR        (1<<1)
#define HAS_BG_PIXMAP       (1<<2)
#define HAS_FONT            (1<<3)

struct Sitem {
  char *name;
  int  flags;
};

Sitem items[] = {
  { "Desktop",         HAS_BG_COLOR | HAS_BG_PIXMAP },
  { "Window",          HAS_BG_COLOR | HAS_FG_COLOR | HAS_BG_PIXMAP | HAS_FONT },
  { "Document",        HAS_BG_COLOR | HAS_FG_COLOR | HAS_BG_PIXMAP | HAS_FONT },
  { "Menu",            HAS_FONT },
  { "Selected items",  HAS_BG_COLOR | HAS_FG_COLOR },
  { "Tooltips",        HAS_BG_COLOR | HAS_FG_COLOR },
  { "Icons",           HAS_FONT }
};

#define NITEMS  (sizeof(items) / sizeof(items[0]))


//----------------------------------------------------------------------

OXSampleView::OXSampleView(const OXWindow *p) :
  OXFrame(p, 300, 200, SUNKEN_FRAME | DOUBLE_BORDER) {

    _pix = None;
    _CreatePixmap();
    _drawGC = new OXGC(GetDisplay(), _id);

    _tbutton[0] = _client->GetPicture("bclose.xpm", bclose_xpm);
    _tbutton[1] = _client->GetPicture("bmax.xpm", bmax_xpm);
    _tbutton[2] = _client->GetPicture("bmin.xpm", bmin_xpm);

    const OResourcePool *res = _client->GetResourcePool();

    _desktopBgndPixel = _client->GetColorByName("turquoise4");

    _frameBgndPixel = _client->GetColor(_defaultFrameBackground);
    _frameFgndPixel = _client->GetResourcePool()->GetFrameFgndColor();
    _hilitePixel = _client->GetHilite(_frameBgndPixel);
    _shadowPixel = _client->GetShadow(_frameBgndPixel);

    //_blackPixel;
    _selBgndPixel = _client->GetColor(_defaultSelectedBackground);
    _selFgndPixel = _client->GetColor(_defaultSelectedForeground);

    _docBgndPixel = _client->GetColor(_defaultDocumentBackground);
    _docFgndPixel = _client->GetColor(_defaultDocumentForeground);

    _menuFont = _client->GetFont((OXFont *) res->GetMenuFont());
    _iconFont = _client->GetFont((OXFont *) res->GetIconFont());
    _statusFont = _client->GetFont((OXFont *) res->GetStatusFont());
    _defaultFont = _client->GetFont((OXFont *) res->GetDefaultFont());
    _tipFont = _client->GetFont((OXFont *) res->GetDefaultFont());
    _docFixedFont = _client->GetFont((OXFont *) res->GetDocumentFixedFont());
    _docPropFont = _client->GetFont((OXFont *) res->GetDocumentPropFont());

    _wmBorderWidth = 5;
    _wmTitleHeight = 17;

    _Draw();
}

OXSampleView::~OXSampleView() {
  if (_pix != None) XFreePixmap(GetDisplay(), _pix);
  delete _drawGC;
  _client->FreePicture(_tbutton[0]);
  _client->FreePicture(_tbutton[1]);
  _client->FreePicture(_tbutton[2]);
}

void OXSampleView::_CreatePixmap() {
  if (_pix != None) XFreePixmap(GetDisplay(), _pix);

  _pixw = _w - 2 * _bw;
  _pixh = _h - 2 * _bw;

  _pix = XCreatePixmap(GetDisplay(), _id, _pixw, _pixh,
                       _client->GetDisplayDepth());
}

void OXSampleView::Resize(int w, int h) {
  OXFrame::Resize(w, h);
  _CreatePixmap();
  _Draw();
}

void OXSampleView::MoveResize(int x, int y, int w, int h) {
  OXFrame::MoveResize(x, y, w, h);
  _CreatePixmap();
  _Draw();
}

void OXSampleView::_Draw() {
  Drawable dst;
  int x0, y0;

  if (_pix != None) {
    dst = _pix;
    x0 = y0 = 0;
  } else {
    dst = _id;
    x0 = y0 = _bw;
  }

  // draw desktop background

  _drawGC->SetForeground(_desktopBgndPixel);
  XFillRectangle(GetDisplay(), dst, _drawGC->GetGC(), x0, y0, _pixw, _pixh);

  // draw an inactive top-level window

  int x = x0 + 40;
  int y = y0 + 10;
  int w = _pixw - 80;
  int h = _pixh - 45;

  _DrawWindow(dst, x, y, w, h, False);

  // draw a second, active, top-level window

  x = x0 + 40 + _wmBorderWidth;
  y = y0 + 10 + _wmBorderWidth + _wmTitleHeight + 1;
  w = _pixw - 60;
  h = _pixh - 45;

  _DrawWindow(dst, x, y, w, h, True);

  if (_pix != None) NeedRedraw(False);
}

void OXSampleView::_DrawWindow(Drawable dst, int x, int y, int w, int h,
                                int active) {

  OFontMetrics fm;

  // 3d border

  _drawGC->SetForeground(_frameBgndPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x, y, x + w - 2, y);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x, y, x, y + h - 2);
  _drawGC->SetForeground(_hilitePixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + 1, y + 1, x + w - 3, y + 1);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + 1, y + 1, x + 1, y + h - 3);

  _drawGC->SetForeground(_shadowPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + 1, y + h - 2, x + w - 2, y + h - 2);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + w - 2, y + h - 2, x + w - 2, y + 1);
  _drawGC->SetForeground(_blackPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x, y + h - 1, x + w - 1, y + h - 1);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + w - 1, y + h - 1, x + w - 1, y);

  // frame background

  _drawGC->SetForeground(_frameBgndPixel);
  XFillRectangle(GetDisplay(), dst, _drawGC->GetGC(),
                 x + 2, y + 2, w - 4, h - 4);

  // title bar

  int fw = _wmBorderWidth;
  int th = _wmTitleHeight;

  _drawGC->SetForeground(active ? _selBgndPixel : _shadowPixel);
  XFillRectangle(GetDisplay(), dst, _drawGC->GetGC(),
                 x + fw, y + fw, w - fw * 2, th);

  _drawGC->SetForeground(active ? _selFgndPixel : _frameBgndPixel);
  OString title(active ? "Active Window" : "Inactive Window");

  const OXFont *titleFont = _client->GetFont("helvetica -12 bold");
  titleFont->GetFontMetrics(&fm);

  _drawGC->SetFont(titleFont->GetId());

  title.Draw(GetDisplay(), dst, _drawGC->GetGC(),
             x + fw + 5, y + fw + fm.ascent + 1);

  _client->FreeFont((OXFont *) titleFont);

  // title bar buttons

  int bw = 16;
  int bh = 14;
  int bx = x + w - fw - 2 - bw;
  int by = y + fw + 2;

  _DrawButton(dst, bx, by, bw, bh, _tbutton[0]);
  bx -= bw + 2;
  _DrawButton(dst, bx, by, bw, bh, _tbutton[1]);
  bx -= bw;
  _DrawButton(dst, bx, by, bw, bh, _tbutton[2]);

  if (!active) return;

  // menu

  _menuFont->GetFontMetrics(&fm);
  int mh = fm.ascent + fm.descent + 7;
  int mx = x + fw + 4;

  _drawGC->SetFont(_menuFont->GetId());
  _drawGC->SetForeground(_frameFgndPixel);

  OHotString str1("&File");
  str1.Draw(GetDisplay(), dst, _drawGC->GetGC(),
            mx, y + fw + th + fm.ascent + 4);
  mx += _menuFont->XTextWidth(str1.GetString()) + 12;

  OHotString str2("&View");
  str2.Draw(GetDisplay(), dst, _drawGC->GetGC(),
            mx, y + fw + th + fm.ascent + 4);
  mx += _menuFont->XTextWidth(str2.GetString()) + 12;

  OHotString str3("&Help");
  str3.Draw(GetDisplay(), dst, _drawGC->GetGC(),
            mx, y + fw + th + fm.ascent + 4);

  // document area

  int dx = x + fw;
  int dy = y + fw + th + mh;
  int dw = w - 2 * fw;
  int dh = h - 2 * fw - th - mh;

  // sunken border

  _drawGC->SetForeground(_shadowPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx, dy, dx + dw - 2, dy);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx, dy, dx, dy + dh - 2);
  _drawGC->SetForeground(_blackPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx + 1, dy + 1, dx + dw - 3, dy + 1);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx + 1, dy + 1, dx + 1, dy + dh - 3);

  _drawGC->SetForeground(_hilitePixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx, dy + dh - 1, dx + dw - 1, dy + dh - 1);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx + dw - 1, dy + dh - 1, dx + dw - 1, dy);
  _drawGC->SetForeground(_frameBgndPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx + 1, dy + dh - 2, dx + dw - 2, dy + dh - 2);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            dx + dw - 2, dy + 1, dx + dw - 2, dy + dh - 2);

  // doc background

  _drawGC->SetForeground(_docBgndPixel);
  XFillRectangle(GetDisplay(), dst, _drawGC->GetGC(),
                 dx + 2, dy + 2, dw - 4, dh - 4);

  // doc text

  _drawGC->SetFont(_docPropFont->GetId());
  _drawGC->SetForeground(_docFgndPixel);

  OString str4("Default document proportional font");
  str4.Draw(GetDisplay(), dst, _drawGC->GetGC(),
            dx + 5, dy + fm.ascent + 5);

  _drawGC->SetFont(_docFixedFont->GetId());

  _docPropFont->GetFontMetrics(&fm);
  th = fm.ascent + fm.descent;

  OString str5("Default document monospaced font");
  str5.Draw(GetDisplay(), dst, _drawGC->GetGC(),
            dx + 5, dy + fm.ascent + 5 + th);

  _docFixedFont->GetFontMetrics(&fm);
  th += fm.ascent + fm.descent;

  _drawGC->SetFont(_docPropFont->GetId());
  _drawGC->SetForeground(_selFgndPixel);
  _drawGC->SetBackground(_selBgndPixel);

  OString str6("  Selected text  ");
  XDrawImageString(GetDisplay(), dst, _drawGC->GetGC(),
                   dx + 5, dy + fm.ascent + 10 + th,
                   str6.GetString(), str6.GetLength());
}

void OXSampleView::_DrawButton(Drawable dst, int x, int y, int w, int h,
                               const OPicture *pic) {

  // 3d border

  _drawGC->SetForeground(_hilitePixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x, y, x + w - 2, y);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x, y, x, y + h - 2);
  _drawGC->SetForeground(_frameBgndPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + 1, y + 1, x + w - 3, y + 1);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + 1, y + 1, x + 1, y + h - 3);

  _drawGC->SetForeground(_shadowPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + 1, y + h - 2, x + w - 2, y + h - 2);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + w - 2, y + h - 2, x + w - 2, y + 1);
  _drawGC->SetForeground(_blackPixel);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x, y + h - 1, x + w - 1, y + h - 1);
  XDrawLine(GetDisplay(), dst, _drawGC->GetGC(),
            x + w - 1, y + h - 1, x + w - 1, y);

  // frame background

  _drawGC->SetForeground(_frameBgndPixel);
  XFillRectangle(GetDisplay(), dst, _drawGC->GetGC(),
                 x + 2, y + 2, w - 4, h - 4);

  // icon

  if (pic) 
    pic->Draw(GetDisplay(), dst, _drawGC->GetGC(), x + 2, y + 2);

}

void OXSampleView::_DoRedraw() {
  OXFrame::DrawBorder();
  if (_pix != None)
    XCopyArea(GetDisplay(), _pix, _id, _drawGC->GetGC(), 
              0, 0, _pixw, _pixh, _bw, _bw);
  else
    _Draw();
}

void OXSampleView::ChangeSettings(Sconfig *settings) {

  //---- colors

  _client->FreeColor(_desktopBgndPixel);
  _desktopBgndPixel = _client->GetColorByName(settings->desktop_bg_color);

  _client->FreeColor(_frameBgndPixel);
  _frameBgndPixel = _client->GetColorByName(settings->frame_bg_color);

  _client->FreeColor(_frameFgndPixel);
  _frameFgndPixel = _client->GetColorByName(settings->frame_fg_color);

  _client->FreeColor(_hilitePixel);
  _hilitePixel = _client->GetHilite(_frameBgndPixel);

  _client->FreeColor(_shadowPixel);
  _shadowPixel = _client->GetShadow(_frameBgndPixel);

    //_blackPixel;
  _client->FreeColor(_selBgndPixel);
  _selBgndPixel = _client->GetColorByName(settings->sel_bg_color);

  _client->FreeColor(_selFgndPixel);
  _selFgndPixel = _client->GetColorByName(settings->sel_fg_color);

  _client->FreeColor(_docBgndPixel);
  _docBgndPixel = _client->GetColorByName(settings->doc_bg_color);

  _client->FreeColor(_docFgndPixel);
  _docFgndPixel = _client->GetColorByName(settings->doc_fg_color);

  // settings->tip_bg_color
  // settings->tip_fg_color

  //---- fonts

  _client->FreeFont((OXFont *) _menuFont);
  _menuFont = _client->GetFont(settings->menu_font);

  // settings->menu_hi_font

  _client->FreeFont((OXFont *) _iconFont);
  _iconFont = _client->GetFont(settings->icon_font);

  _client->FreeFont((OXFont *) _statusFont);
  _statusFont = _client->GetFont(settings->status_font);

  _client->FreeFont((OXFont *) _defaultFont);
  _defaultFont = _client->GetFont(settings->default_font);

  //  _tipFont = _client->GetResourcePool()->GetDefaultFont();

  _client->FreeFont((OXFont *) _docFixedFont);
  _docFixedFont = _client->GetFont(settings->doc_fixed_font);

  _client->FreeFont((OXFont *) _docPropFont);
  _docPropFont = _client->GetFont(settings->doc_prop_font);

  //---- pixmaps

  // settings->frame_bg_pixmap
  // settings->doc_bg_pixmap

  _Draw();
}

//----------------------------------------------------------------------

OXAppearanceTab::OXAppearanceTab(const OXWindow *tab, const OPicture *mon) :
  OXCompositeFrame(tab, 100, 100, VERTICAL_FRAME) {

    CommonCLayout = new OLayoutHints(LHINTS_CENTER_X | LHINTS_CENTER_Y,
                                     2, 2, 2, 2);

    AddFrame(_sample = new OXSampleView(this),
             new OLayoutHints(LHINTS_EXPAND_X | LHINTS_TOP, 5, 5, 5, 5));

    OLayoutHints *lhe = new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X,
                                         5, 5, 1, 1);
    OLayoutHints *lhf = new OLayoutHints(LHINTS_TOP | LHINTS_LEFT,
                                         5, 5, 5, 1);
    OLayoutHints *lhi = new OLayoutHints(LHINTS_LEFT | LHINTS_EXPAND_Y,
                                         0, 5, 0, 0);
    OLayoutHints *lhb = new OLayoutHints(LHINTS_EXPAND_ALL,
                                         5, 0, 0, 0);


    AddFrame(new OXLabel(this, new OHotString("&Scheme:")), lhf);
    OXHorizontalFrame *hf = new OXHorizontalFrame(this);
    AddFrame(hf, lhe);
    hf->AddFrame(new OXDDListBox(hf, -1), lhi);
    hf->AddFrame(new OXTextButton(hf, new OHotString("Sa&ve as..."), -1), lhb);
    hf->AddFrame(new OXTextButton(hf, new OHotString("&Delete"), -1), lhb);

    AddFrame(new OXLabel(this, new OHotString("&Item:")), lhf);
    hf = new OXHorizontalFrame(this);
    AddFrame(hf, lhe);
    hf->AddFrame(ddlb = new OXDDListBox(hf, 4100), lhi);
    //hf->AddFrame(new OXSpinner(hf, "", -1), lhi);
    hf->AddFrame(fgtr = new OXColorSelect(hf, OColor(255, 0, 0), 4101), lhi);
    hf->AddFrame(bgtr = new OXColorSelect(hf, OColor(255, 0, 0), 4102), lhi);

    for (int i = 0; i < NITEMS; ++i) {
      ddlb->AddEntry(new OString(items[i].name), i);
    }
    ddlb->Select(0);
    ddlb->Associate(this);

    fgtr->Associate(this);
    bgtr->Associate(this);

    fontb = new OXTextButton(hf, new OHotString("  &Font...  "), 4103);
    hf->AddFrame(fontb, new OLayoutHints(LHINTS_LEFT | LHINTS_TOP, 5, 0, 0, 0));

    fontb->Associate(this);

    InitServer();

    _currentFont = NULL;
    _currentFG = _currentBG = NULL;

    ddlb->Select(0);
    SelItem(0);
}

OXAppearanceTab::~OXAppearanceTab() {
}

int OXAppearanceTab::InitServer() {

  _XCLASS_RESOURCES = XInternAtom(GetDisplay(), "_XCLASS_RESOURCES", False);

  root_colormap = _client->GetDefaultColormap();
  const OResourcePool *res = _client->GetResourcePool();

  strcpy(settings.default_font, FontName(res->GetDefaultFont()));
  strcpy(settings.menu_font, FontName(res->GetMenuFont()));
  strcpy(settings.menu_hi_font, FontName(res->GetMenuHiliteFont()));
  strcpy(settings.doc_fixed_font, FontName(res->GetDocumentFixedFont()));
  strcpy(settings.doc_prop_font, FontName(res->GetDocumentPropFont()));
  strcpy(settings.icon_font, FontName(res->GetIconFont()));
  strcpy(settings.status_font, FontName(res->GetStatusFont()));
  
  strcpy(settings.frame_bg_color, ColorName(res->GetFrameBgndColor()));
  strcpy(settings.frame_fg_color, ColorName(res->GetFrameFgndColor()));

  strcpy(settings.doc_bg_color, ColorName(res->GetDocumentBgndColor()));
  strcpy(settings.doc_fg_color, ColorName(res->GetDocumentFgndColor()));

  strcpy(settings.sel_bg_color, ColorName(res->GetSelectedBgndColor()));
  strcpy(settings.sel_fg_color, ColorName(res->GetSelectedFgndColor()));

  strcpy(settings.tip_bg_color, ColorName(res->GetTipBgndColor()));
  strcpy(settings.tip_fg_color, ColorName(res->GetTipFgndColor()));

  strcpy(settings.desktop_bg_color, "turquoise4");
  
  strcpy(settings.frame_bg_pixmap, PixmapName(res->GetFrameBckgndPicture()));
  strcpy(settings.doc_bg_pixmap, PixmapName(res->GetDocumentBckgndPicture()));

  _sample->ChangeSettings(&settings);

  //PrintSettings();
  SetProperty();

  return True;
}

int OXAppearanceTab::SetProperty() {
  char tmp[8192];

  //strcpy(settings.default_font, "lucida -12");

  sprintf(tmp, "normal font = %s\n"
               "menu font = %s\n"
               "menu highlight font = %s\n"
               "small font = %s\n"
               "proportional font = %s\n"
               "monospaced font = %s\n"
               "fore color = %s\n"
               "back color = %s\n"
               "hifore color = %s\n"
               "hiback color = %s\n"
               "doc fore color = %s\n"
               "doc back color = %s\n"
               "tip fore color = %s\n"
               "tip back color = %s\n"
               "desktop back color = %s\n"
               "background pixmap = %s\n"
               "document background pixmap = %s\n",
               settings.default_font,
               settings.menu_font,
               settings.menu_hi_font,
               settings.icon_font,
               settings.doc_prop_font,
               settings.doc_fixed_font,
               settings.frame_fg_color,
               settings.frame_bg_color,
               settings.sel_fg_color,
               settings.sel_bg_color,
               settings.doc_fg_color,
               settings.doc_bg_color,
               settings.tip_fg_color,
               settings.tip_bg_color,
               settings.desktop_bg_color,
               settings.frame_bg_pixmap,
               settings.doc_bg_pixmap);

  XChangeProperty(GetDisplay(), _client->GetRoot()->GetId(),
                  _XCLASS_RESOURCES, XA_STRING,
                  8, PropModeReplace, (unsigned char *) tmp, strlen(tmp)+1);

  if (1) {  // _fvwmModuleMode;
    unsigned long bg_pixel, shadow_pixel;

    // quick hack!
    bg_pixel = _client->GetColorByName(settings.frame_bg_color);
    shadow_pixel = _client->GetShadow(bg_pixel);

    sprintf(tmp, "DefaultColors %s %s %s %s\n",
                 settings.frame_fg_color,
                 settings.frame_bg_color,
                 settings.frame_bg_color,
                 ColorName(shadow_pixel));

    _client->FreeColor(bg_pixel);
    _client->FreeColor(shadow_pixel);

    OFvwmMessage msg(MSG_FVWM, MSG_SEND, 0, 0, NULL, tmp);
    SendMessage(_msgObject, &msg);

    sprintf(tmp, "HilightColors %s %s\n",
                 settings.sel_fg_color,
                 settings.sel_bg_color);

    OFvwmMessage msg1(MSG_FVWM, MSG_SEND, 0, 0, NULL, tmp);
    SendMessage(_msgObject, &msg1);

    // hmmm...
    OFvwmMessage msg2(MSG_FVWM, MSG_SEND, 0, 0, NULL, "Recapture\n");
    SendMessage(_msgObject, &msg2);
  }
}

char *OXAppearanceTab::ColorName(unsigned long color) {
  static char str[256];
  XColor xcolor;

  xcolor.pixel = color;
  XQueryColor(GetDisplay(), root_colormap, &xcolor);
  sprintf(str, "#%04x%04x%04x", xcolor.red, xcolor.green, xcolor.blue);

  return str;
}

char *OXAppearanceTab::ColorName(OColor color) {
  static char str[256];

  sprintf(str, "#%02x%02x%02x", color.GetR(), color.GetG(), color.GetB());

  return str;
}

char *OXAppearanceTab::FontName(const OXFont *font) {
  static char str[256];

  if (font) {
    strcpy(str, font->NameOfFont());
  } else {
    strcpy(str, "None");
  }

  return str;
}

char *OXAppearanceTab::PixmapName(const OPicture *pic) {
  static char str[256];

  if (pic) {
    strcpy(str, pic->GetName());
  } else {
    strcpy(str, "None");
  }

  return str;
}

void OXAppearanceTab::PrintSettings() {

  printf("Default font:       \"%s\"\n", settings.default_font);
  printf("Menu font:          \"%s\"\n", settings.menu_font);
  printf("Mehu hilite font:   \"%s\"\n", settings.menu_hi_font);
  printf("Doc fixed font:     \"%s\"\n", settings.doc_fixed_font);
  printf("Doc prop font:      \"%s\"\n", settings.doc_prop_font);
  printf("Icon font:          \"%s\"\n", settings.icon_font);
  printf("Status font:        \"%s\"\n", settings.status_font);
  printf("\n");
  printf("Frame bgnd color:   \"%s\"\n", settings.frame_bg_color);
  printf("Frame fgnd color:   \"%s\"\n", settings.frame_fg_color);
  printf("Doc bgnd color:     \"%s\"\n", settings.doc_bg_color);
  printf("Doc fgnd color:     \"%s\"\n", settings.doc_fg_color);
  printf("Sel bgnd color:     \"%s\"\n", settings.sel_bg_color);
  printf("Sel fgnd color:     \"%s\"\n", settings.sel_fg_color);
  printf("Tip bgnd color:     \"%s\"\n", settings.tip_bg_color);
  printf("Tip fgnd color:     \"%s\"\n", settings.tip_fg_color);
  printf("Desktop bgnd color: \"%s\"\n", settings.desktop_bg_color);
  printf("\n");
  printf("Frame bgnd pixmap:  \"%s\"\n", settings.frame_bg_pixmap);
  printf("Doc bgnd pixmap:    \"%s\"\n", settings.doc_bg_pixmap);

}

int OXAppearanceTab::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;
  OColorSelMessage *cmsg = (OColorSelMessage *) msg;
  int  rtc;
  char buf[100];

  switch (msg->type) {
    case MSG_BUTTON:
      switch (msg->action) {
        case MSG_CLICK:
          if (wmsg->id == 4103 && _currentFont) {
            OString fontName(_currentFont);
            new OXFontDialog(_client->GetRoot(), GetTopLevel(), &fontName);
            strcpy(_currentFont, fontName.GetString());
            if (_currentFont == settings.icon_font)
              strcpy(settings.status_font, settings.icon_font);
            _sample->ChangeSettings(&settings);
          } else if (wmsg->id == ID_APPLY) {
            SetProperty();
            //PrintSettings();
          }
          break;
      }
      break;

    case MSG_DDLISTBOX:
      switch (msg->action) {
        case MSG_CLICK:
          SelItem(ddlb->GetSelected());
          break;
      }
      break;

    case MSG_COLORSEL:
      switch (msg->action) {
        case MSG_CLICK:
          if (cmsg->id == 4101) {
            if (_currentFG) strcpy(_currentFG, ColorName(cmsg->color));
          } else if (cmsg->id == 4102) {
            if (_currentBG) strcpy(_currentBG, ColorName(cmsg->color));
          }
          _sample->ChangeSettings(&settings);
          break;
      }
      break;

    default:
      break;
  }
  return rtc;
}

int OXAppearanceTab::SelItem(int ix) {

  int flg = items[ix].flags;

  if (flg & HAS_FG_COLOR) fgtr->Enable(); else fgtr->Disable();
  if (flg & HAS_BG_COLOR) bgtr->Enable(); else bgtr->Disable();
  if (flg & HAS_FONT) fontb->Enable(); else fontb->Disable();

  switch (ix) {
    case 0:  // desktop
      bgtr->SetColor(_client->GetColorByName("turquoise4"));
      _currentFont = NULL;
      _currentFG = NULL;
      _currentBG = settings.desktop_bg_color;
      break;

    case 1:  // window
      fgtr->SetColor(_client->GetColorByName(settings.frame_fg_color));
      bgtr->SetColor(_client->GetColorByName(settings.frame_bg_color));
      _currentFont = settings.default_font;
      _currentFG = settings.frame_fg_color;
      _currentBG = settings.frame_bg_color;
      break;

    case 2:  // document
      fgtr->SetColor(_client->GetColorByName(settings.doc_fg_color));
      bgtr->SetColor(_client->GetColorByName(settings.doc_bg_color));
      _currentFont = settings.doc_prop_font;
      _currentFG = settings.doc_fg_color;
      _currentBG = settings.doc_bg_color;
      break;

    case 3:  // menu
      _currentFont = settings.menu_font;
      _currentFG = NULL;
      _currentBG = NULL;
      break;

    case 4:  // selected items
      fgtr->SetColor(_client->GetColorByName(settings.sel_fg_color));
      bgtr->SetColor(_client->GetColorByName(settings.sel_bg_color));
      _currentFont = NULL;
      _currentFG = settings.sel_fg_color;
      _currentBG = settings.sel_bg_color;
      break;

    case 5:  // tooltips
      fgtr->SetColor(_client->GetColorByName(settings.tip_fg_color));
      bgtr->SetColor(_client->GetColorByName(settings.tip_bg_color));
      _currentFont = NULL;
      _currentFG = settings.tip_fg_color;
      _currentBG = settings.tip_bg_color;
      break;

    case 6:  // icons
      _currentFont = settings.icon_font;
      _currentFG = NULL;
      _currentBG = NULL;
      break;
  }

  return True;
}

int OXAppearanceTab::LoadSchemes() {
}

int OXAppearanceTab::SaveSchemes() {
}
