#include <stdlib.h>
#include <stdio.h>

#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OXToolBar.h>
#include <xclass/OXStatusBar.h>
#include <xclass/OXMenu.h>
#include <xclass/OX3dLines.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OString.h>


//---------------------------------------------------------------------

class OXMain : public OXMainFrame {
public:
  OXMain(const OXWindow *p, int w, int h);
  virtual ~OXMain();

  virtual int  ProcessMessage(OMessage *msg);
  virtual void CloseWindow();

  void SetWindowTitle(char *title);
  void UpdateStatus();

  void ReadFile(char *fname);
  void WriteFile(char *fname);

  void DoOpen();
  void DoSave(char *fname);
  void DoPrint();
  void DoAbout();
  void DoToggleToolBar();
  void DoToggleStatusBar();

  void ErrorMsg(int icon_type, char *msg);

protected:
  OXPopupMenu *_MakePopup(struct _popup *);

  OXToolBar *_toolBar;
  OXStatusBar *_statusBar;
  OXHorizontal3dLine *_toolBarSep;
  OXCanvas *_canvas;
  OXCompositeFrame *_container;

  OLayoutHints *_menuBarLayout, *_menuBarItemLayout;

  OXMenuBar *_menuBar;
  OXPopupMenu *_menuFile, *_menuEdit, *_menuView, *_menuHelp;
};
