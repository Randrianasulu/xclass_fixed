/**************************************************************************

    This file is part of Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include <xclass/OXClient.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OX3dLines.h>
#include <xclass/OXButton.h>
#include <xclass/OXMenu.h>
#include <xclass/OXLabel.h>
#include <xclass/OXFileDialog.h>
#include <xclass/OXToolBar.h>
#include <xclass/OXStatusBar.h>
#include <xclass/OXMsgBox.h>
#include <xclass/version.h>
#include <xclass/OXAboutDialog.h>
#include <xclass/OXFontDialog.h>
#include <xclass/utils.h>

#include "df-openl.xpm"
#include "df-openr.xpm"
#include "df-dodiff.xpm"
#include "df-undodiff.xpm"
#include "df-prev.xpm"
#include "df-center.xpm"
#include "df-next.xpm"

#include "OXDiff.h"


#define XCDIFF_VERSION    "V0.6"


#define MENU_DISABLED     (1<<0)
#define MENU_CHECKED      (1<<1)
#define MENU_RCHECKED     (1<<2)
 
#define M_FILE_OPENLEFT   1
#define M_FILE_OPENRIGHT  2
#define M_FILE_EXIT       3

#define M_DIFF_DO         11
#define M_DIFF_PREVIOUS   12
#define M_DIFF_CENTER     13
#define M_DIFF_NEXT       14
#define M_DIFF_UNDO       15

#define M_OPTIONS_FONT    21
#define M_OPTIONS_COLORS  22
#define M_OPTIONS_LINES   23
#define M_OPTIONS_SAVE    24

#define M_HELP_CONTENTS   31
#define M_HELP_SEARCH     32
#define M_HELP_ABOUT      33


//----- Popup stuff...
struct _popup {
  OXPopupMenu *ptr;
  struct {
    char *name;
    int  id, state;
    struct _popup *popup_ref;
  } popup[20];
};

SToolBarData tb_data[] = {
  { "df-openl.xpm",    df_openl_xpm,    "Open left",            BUTTON_NORMAL, M_FILE_OPENLEFT,  NULL },
  { "df-openr.xpm",    df_openr_xpm,    "Open right",           BUTTON_NORMAL, M_FILE_OPENRIGHT, NULL },
  { "",	               NULL,            "",                     0,             -1,               NULL },
  { "df-dodiff.xpm",   df_dodiff_xpm,   "Do differences",       BUTTON_NORMAL, M_DIFF_DO,        NULL },
  { "df-undodiff.xpm", df_undodiff_xpm,	"Undo differences",     BUTTON_NORMAL, M_DIFF_UNDO,      NULL },
  { "",                NULL,            "",                     0,             -1,               NULL },
  { "df-next.xpm",     df_next_xpm,     "Next differences",     BUTTON_NORMAL, M_DIFF_NEXT,      NULL },
  { "df-center.xpm",   df_center_xpm,   "Center differences",   BUTTON_NORMAL, M_DIFF_CENTER,    NULL },
  { "df-prev.xpm",     df_prev_xpm,     "Previous differences", BUTTON_NORMAL, M_DIFF_PREVIOUS,  NULL },
  { NULL,              NULL,            NULL,                   0,             0,                NULL }
};

struct _popup file_popup = {
  { NULL }, {
  { "&Open left...",  M_FILE_OPENLEFT,  0, NULL },
  { "Open &right...", M_FILE_OPENRIGHT, 0, NULL },
  { "",               -1,               0, NULL },
  { "E&xit",          M_FILE_EXIT,      0, NULL },
  { NULL,             -1,               0, NULL } } };

struct _popup diff_popup = {
  { NULL }, {
  { "&Do",       M_DIFF_DO,       MENU_DISABLED, NULL },
  { "&Undo",     M_DIFF_UNDO,     0,             NULL },
  { "&Previous", M_DIFF_PREVIOUS, MENU_DISABLED, NULL },
  { "&Center",   M_DIFF_CENTER,   MENU_DISABLED, NULL },
  { "&Next",     M_DIFF_NEXT,     MENU_DISABLED, NULL },
  { NULL,        -1,              0,             NULL } } };

struct _popup options_popup = {
  { NULL }, {
  { "&Font...",	          M_OPTIONS_FONT,   0, NULL },
  { "&Colors...",         M_OPTIONS_COLORS, 0, NULL },
  { "&Show Line Numbers", M_OPTIONS_LINES,  0, NULL },
  { "",                   -1,               0, NULL },
  { "&Save Options",      M_OPTIONS_SAVE,   0, NULL },
  { NULL,                 -1,               0, NULL } } };
 
struct _popup help_popup = {
  { NULL }, {
  { "&Contents...", M_HELP_CONTENTS, 0, NULL },
  { "&Search...",   M_HELP_SEARCH,   0, NULL },
  { "",             -1,              0, NULL },
  { "&About...",    M_HELP_ABOUT,    0, NULL },
  { NULL,           -1,              0, NULL } } };
 

//---------------------------------------------------------------------

class OXDiffMainFrame : public OXMainFrame {
public:
  OXDiffMainFrame(const OXWindow *p, int w, int h, int, char **);
  virtual ~OXDiffMainFrame();

  virtual int ProcessMessage(OMessage *msg);

protected:
  OXPopupMenu *_MakePopup(struct _popup *);
  void DoToggleLineNum();
  char *DoOpen(char *path);
  void DoDiff();
  void UnDoDiff();
  void ShowDiff(int);
  void EnableDiffMenu();
  void DisableDiffMenu();
  void ErrorMsg(int icon_type, char *msg);
  void About();

  OXStatusBar *_statusBar;
  OXToolBar *_toolBar;
  OXHorizontal3dLine *_toolBarSep;
  OXDiff *_diffView;

  OLayoutHints *_menuBarLayout, *_menuBarItemLayout, *_menuBarHelpLayout;
  OXMenuBar *_menuBar;
  OXPopupMenu *_menuFile, *_menuDiff, *_menuOptions, *_menuHelp;
  OXDDListBox *_diffBox;
  OXLabel *_diffLabel;
  
  char *_leftFile, _leftPath[PATH_MAX];
  char *_rightFile, _rightPath[PATH_MAX];
};


//---------------------------------------------------------------------

OMimeTypes *MimeTypeList;  // currently we need this for OXFileDialog

int main(int argc, char **argv) {
  char mimerc[PATH_MAX];

  OXClient clientX;

  OXDiffMainFrame *mainw = new OXDiffMainFrame(clientX.GetRoot(), 10, 10,
                                               argc, argv);

  sprintf(mimerc, "%s/.mime.types", getenv("HOME"));
  MimeTypeList = new OMimeTypes(&clientX, mimerc);

  mainw->Resize(600, 400);
  mainw->MapWindow();

  clientX.Run();
  delete MimeTypeList;
}

OXDiffMainFrame::OXDiffMainFrame(const OXWindow *p, int w, int h,
			 int argc, char **argv) :
  OXMainFrame(p, w, h) {
  char tmp[BUFSIZ];

  getcwd(_leftPath, PATH_MAX);
  getcwd(_rightPath, PATH_MAX);

  _leftFile = _rightFile = NULL;
  
  _menuBarLayout = new OLayoutHints(LHINTS_TOP | LHINTS_LEFT | LHINTS_EXPAND_X, 0, 0, 1, 1);
  _menuBarItemLayout = new OLayoutHints(LHINTS_TOP | LHINTS_LEFT, 0, 4, 0, 0);
  _menuBarHelpLayout = new OLayoutHints(LHINTS_TOP | LHINTS_RIGHT);

  _menuFile = _MakePopup(&file_popup);
  _menuDiff = _MakePopup(&diff_popup);
  _menuOptions = _MakePopup(&options_popup);
  _menuHelp = _MakePopup(&help_popup);

  _menuFile->Associate(this);
  _menuDiff->Associate(this);
  _menuOptions->Associate(this);
  _menuHelp->Associate(this);

  _menuBar = new OXMenuBar(this, 1, 1, HORIZONTAL_FRAME);
  _menuBar->AddPopup(new OHotString("&File"), _menuFile, _menuBarItemLayout);
  _menuBar->AddPopup(new OHotString("&Difference"), _menuDiff, _menuBarItemLayout);
  _menuBar->AddPopup(new OHotString("&Options"), _menuOptions, _menuBarItemLayout);
  _menuBar->AddPopup(new OHotString("&Help"), _menuHelp, _menuBarItemLayout);

  _toolBarSep = new OXHorizontal3dLine(this);
  _toolBar = new OXToolBar(this);
  _toolBar->AddButtons(tb_data);

  _diffBox = new OXDDListBox(_toolBar, 100);
  _diffLabel = new OXLabel(_toolBar, new OString("Difference 0 of 0"));
  _toolBar->AddFrame(_diffBox, new OLayoutHints(LHINTS_LEFT | LHINTS_EXPAND_Y,
                                        10, 0, 0, 0));
  _toolBar->AddFrame(_diffLabel, new OLayoutHints(LHINTS_LEFT | LHINTS_CENTER_Y,
                                        10, 0, 0, 0));
  _diffBox->Resize(120, 20);
  _diffBox->Associate(this);
  
  _diffBox->Select(0);
  
  AddFrame(_menuBar, _menuBarLayout);
  AddFrame(_toolBarSep, _menuBarLayout);
  AddFrame(_toolBar, new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X, 
                                      0, 0, 1, 3));

  _statusBar = new OXStatusBar(this);
  AddFrame(_statusBar, new OLayoutHints(LHINTS_BOTTOM | LHINTS_EXPAND_X,
                                     0, 0, 3, 0));

  _statusBar->SetText(0, new OString("XC Diff "XCDIFF_VERSION));
  
  _diffView = new OXDiff(this, 10, 10);
  AddFrame(_diffView, new OLayoutHints(LHINTS_EXPAND_Y | LHINTS_EXPAND_X));
                                     
  _diffView->Resize(_diffView->GetDefaultSize());

  SetWindowName("XC Diff "XCDIFF_VERSION);
  SetClassHints("XCDiff", "XCDiff");

  MapSubwindows();

  if (argc >= 3) {
    _leftFile = new char[strlen(argv[1])+1];
    strcpy(_leftFile, argv[1]);
    _rightFile = new char[strlen(argv[2])+1];
    strcpy(_rightFile, argv[2]);
    _diffView->SetLeftFile(_leftFile);
    _diffView->SetRightFile(_rightFile);
  }
  DoDiff();
}

OXDiffMainFrame::~OXDiffMainFrame() {

  delete _menuBarLayout;
  delete _menuBarItemLayout;
  delete _menuBarHelpLayout; 

  delete _menuFile;
  delete _menuDiff;
  delete _menuOptions;
  delete _menuHelp;

  if (_leftFile) delete[] _leftFile;
  if (_rightFile) delete[] _rightFile;

}

int OXDiffMainFrame::ProcessMessage(OMessage *msg) {
  char *_ptr;
  int curr, val;
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;
  
  switch (msg->action) {
    case MSG_CLICK:
      switch(msg->type) {
        case MSG_BUTTON:
        case MSG_MENU:
          switch(wmsg->id) {
            case M_OPTIONS_LINES:
              DoToggleLineNum();
              break;
              
            case M_FILE_OPENLEFT:
              if ((_ptr = DoOpen(_leftPath)) != NULL) {
               	if (_leftFile) delete[] _leftFile;
             	_leftFile = _ptr;
              	_diffView->SetLeftFile(_leftFile);
              	if (_diffView->CanDoDiff())
                  _menuDiff->EnableEntry(M_DIFF_DO);
              	else
                  _menuDiff->DisableEntry(M_DIFF_DO);
              }
              break;

            case M_FILE_OPENRIGHT:
              if ((_ptr = DoOpen(_rightPath)) != NULL) {
              	if (_rightFile) delete[] _rightFile;
              	_rightFile = _ptr;
              	_diffView->SetRightFile(_rightFile);
              	if (_diffView->CanDoDiff())
                  _menuDiff->EnableEntry(M_DIFF_DO);
                else
                  _menuDiff->DisableEntry(M_DIFF_DO);
              }
              break;

            case M_FILE_EXIT:
              CloseWindow();
              exit(0);
              break;

            case M_DIFF_PREVIOUS:
              curr = _diffBox->GetSelected();
              if (--curr == 0) curr = _diffView->GetNumDiffs();
              ShowDiff(curr);
              break;

            case M_DIFF_NEXT:
              curr = _diffBox->GetSelected();
              if (++curr > _diffView->GetNumDiffs()) curr = 1;
              ShowDiff(curr);
              break;

            case M_DIFF_CENTER:
              _diffView->CenterDiff();
              break;

            case M_DIFF_DO:
              DoDiff();
              break;

            case M_DIFF_UNDO:
              UnDoDiff();
              break;

            case M_OPTIONS_FONT:
              {
              OString f(_diffView->GetFont()->NameOfFont());
              new OXFontDialog(_client->GetRoot(), this, &f);
              _diffView->SetFont(_client->GetFont(f.GetString()));
              }
              break;

            case M_HELP_ABOUT:
              About();
              break;
          }
          break;

        case MSG_DDLISTBOX:
          ShowDiff(_diffBox->GetSelected());
          break;
      }
      break;
  }

  return True;
}

void OXDiffMainFrame::ShowDiff(int i) {
  int start, end, type;
  char buf1[128];
	
  int num = _diffView->GetNumDiffs();
  if (i <= num) {
    _diffBox->Select(i);
    _diffView->ShowDiff(i);
    sprintf(buf1, "Difference %d of %d", i, num);
    _diffLabel->SetText(new OString(buf1));
    _diffLabel->Resize(_diffLabel->GetDefaultSize());
  }
}

void OXDiffMainFrame::ErrorMsg(int icon_type, char *msg) {
  OString stitle((icon_type == MB_ICONSTOP) ? "Error" : "Warning");
  OString smsg(msg);
  new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
               icon_type, ID_OK);
}

void OXDiffMainFrame::DoDiff() {
  int i, num;
  char buf1[128], buf2[128];
  if (_diffView->DoDiff() == 1) {
    num = _diffView->GetNumDiffs();
    _diffBox->RemoveAllEntries();
    for (i=1; i<=num; i++)
      _diffBox->AddEntry(new OString(_diffView->GetDiffStr(i)), i);

    if (num) {
      sprintf(buf1, "Difference 1 of %d", num);
      sprintf(buf2, "XC Diff found %d differences", num);
    } else {
      sprintf(buf1, "Difference 0 of 0");
      sprintf(buf2, "XC Diff found no differences");
    }
    _diffLabel->SetText(new OString(buf1));
    _diffLabel->Resize(_diffLabel->GetDefaultSize());
    _statusBar->SetText(0, new OString(buf2)); 
    EnableDiffMenu();
    ShowDiff(1);
  }
}

void OXDiffMainFrame::UnDoDiff() {
  _diffView->UnDoDiff();
  _diffBox->RemoveAllEntries();
  _diffBox->Select(0);
  char buf1[128];
  sprintf(buf1, "Difference 0 of 0");
  _diffLabel->SetText(new OString(buf1));
  _diffLabel->Resize(_diffLabel->GetDefaultSize());
  sprintf(buf1, "XC Diff "XCDIFF_VERSION);
  _statusBar->SetText(0, new OString(buf1)); 
  DisableDiffMenu();
}

void OXDiffMainFrame::EnableDiffMenu() {
  _menuDiff->EnableEntry(M_DIFF_PREVIOUS);
  _menuDiff->EnableEntry(M_DIFF_CENTER);
  _menuDiff->EnableEntry(M_DIFF_NEXT);
}

void OXDiffMainFrame::DisableDiffMenu() {
  _menuDiff->DisableEntry(M_DIFF_PREVIOUS);
  _menuDiff->DisableEntry(M_DIFF_CENTER);
  _menuDiff->DisableEntry(M_DIFF_NEXT);
}

char *OXDiffMainFrame::DoOpen(char *path) {
  OFileInfo fi;
  FILE *fp;
  char *_filename = NULL;
  
  chdir(path);
  fi.MimeTypesList = MimeTypeList;
  fi.file_types = NULL;
  new OXFileDialog(_client->GetRoot(), this, FDLG_OPEN, &fi);
  if (fi.filename) {
    if ((fp = fopen(fi.filename, "r")) == NULL) {
      ErrorMsg(MB_ICONSTOP, "Could not open file.");
    } else {
      _filename = new char[strlen(fi.ini_dir)+strlen(fi.filename)+2];
      sprintf(_filename, "%s/%s", fi.ini_dir, fi.filename);
      fclose(fp);
    }
  }
  getcwd(path, PATH_MAX);
  return _filename;
}

OXPopupMenu *OXDiffMainFrame::_MakePopup(struct _popup *p) {
 
  OXPopupMenu *popup = new OXPopupMenu(_client->GetRoot());
 
  for (int i=0; p->popup[i].name != NULL; ++i) {
    if (strlen(p->popup[i].name) == 0) {
      popup->AddSeparator();
    } else {
      if (p->popup[i].popup_ref == NULL) {
        popup->AddEntry(new OHotString(p->popup[i].name), p->popup[i].id);
      } else {
        struct _popup *p1 = p->popup[i].popup_ref;
        popup->AddPopup(new OHotString(p->popup[i].name), p1->ptr);
      }
      if (p->popup[i].state & MENU_DISABLED) popup->DisableEntry(p->popup[i].id);
      if (p->popup[i].state & MENU_CHECKED) popup->CheckEntry(p->popup[i].id);
      if (p->popup[i].state & MENU_RCHECKED) popup->RCheckEntry(p->popup[i].id,
                                                                p->popup[i].id,
                                                                p->popup[i].id);
    }
  }
  p->ptr = popup;
  return popup;
}
 
void OXDiffMainFrame::DoToggleLineNum() {
  if (_menuOptions->IsEntryChecked(M_OPTIONS_LINES)){ 
    _menuOptions->UnCheckEntry(M_OPTIONS_LINES);
    _diffView->SetLineNumOn(0);
  } else {
    _menuOptions->CheckEntry(M_OPTIONS_LINES);
    _diffView->SetLineNumOn(1);
  }
}

void OXDiffMainFrame::About() {
  OAboutInfo info;

  info.wname = "About xcdiff";
  info.title = "XCDiff "XCDIFF_VERSION"\nA frontend to the \"diff\" command";
  info.copyright = "Copyright � 1998-1999 Matzka Gerald";
  info.text = "This program is free software; you can redistribute it "
              "and/or modify it under the terms of the GNU "
              "General Public License.";

  new OXAboutDialog(_client->GetRoot(), this, &info);
}
