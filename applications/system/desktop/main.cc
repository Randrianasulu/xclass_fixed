/**************************************************************************

    This file is part of xclass' desktop manager.
    Copyright (C) 1996-2000 David Barth, Hector Peraza.

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
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OXMenu.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OString.h>
#include <xclass/OPicture.h>
#include <xclass/OResourcePool.h>
#include <xclass/OXPropertiesDialog.h>
#include <xclass/utils.h>

#include "OXDesktopIcon.h"
#include "OXDesktopContainer.h"

#include "main.h"


//-------------------------------------------------------------------

OXClient *clientX;
OXDesktopMain *mainWindow;

OXDesktopContainer *fileWindow;
OXPopupMenu *objectMenu;
OXPopupMenu *sendToMenu;

char *AppName;

ODNDmanager *dndManager = NULL;

Atom URI_list = None;

//#define TRAP_ERRORS

#ifdef TRAP_ERRORS
int ErrorHandler(Display *dpy, XErrorEvent *event) {
  char text[2048];

  XGetErrorText(dpy, event->error_code, text, 2048);
  fprintf(stderr, "Request %d, Error: %s\n",
                  event->request_code, text);

  return 0;
}
#endif

int main(int argc, char *argv[]) {
  char *p;

  if ((p = strrchr(argv[0], '/')) == NULL)
    AppName = argv[0];
  else
    AppName = ++p;

  clientX = new OXClient;

#ifdef TRAP_ERRORS
  XSynchronize(clientX->GetDisplay(), True);
  XSetErrorHandler(ErrorHandler);
#endif

  mainWindow = new OXDesktopMain(clientX->GetRoot(), 400, 200);
  mainWindow->MapWindow();

  clientX->Run();

  return 0;
}

OXDesktopMain::OXDesktopMain(const OXWindow *p, int w, int h) :
  OXMainFrame(p, w, h) {

    _dndTypeList = new Atom[2];

    _dndTypeList[0] = XInternAtom(GetDisplay(), "text/uri-list", False);
    _dndTypeList[1] = NULL;

    URI_list = _dndTypeList[0];

    _dndManager = new ODNDmanager(_client, this, _dndTypeList);

    dndManager = _dndManager;

    if (!_dndManager->SetRootProxy()) {
      int retc;

      new OXMsgBox(_client->GetRoot(), NULL,
                   new OString("Desktop manager"),
                   new OString("Could not grab the desktop window for "
                               "drag-and-drop operations.\n"
                               "Another desktop manager is probably running.\n"
                               "Continue anyway?"),
                   MB_ICONSTOP, ID_YES | ID_NO, &retc);

      if (retc != ID_YES) exit(1);
    }

    const char *user_root = _client->GetResourcePool()->GetUserRoot();

    char *desktop_dir = new char[strlen(user_root)+9];
    sprintf(desktop_dir, "%s/desktop", user_root);

    if (access(desktop_dir, F_OK)) {
      // probably this is the first time this program is run under this
      // user account: create the necessary dirs here, put some
      // useful stuff into them, warn the user, etc...
      int errc = _CreateDesktop(desktop_dir);

      if (errc) {
        char msg[256];

        sprintf(msg, "Could not create desktop directory\n"
                     "\"%s\":\n%s", desktop_dir, strerror(errc));
        new OXMsgBox(_client->GetRoot(), NULL,
                     new OString("Desktop manager"),
                     new OString(msg), MB_ICONSTOP, ID_CLOSE);
        FatalError(msg);
      }
    }

    _MakeMenus();

    fileWindow = new OXDesktopContainer(this, 500, 250, CHILD_FRAME);
    fileWindow->Associate(this);

    fileWindow->ChangeDirectory(desktop_dir);

    fileWindow->Init();
    // We call Save() immediately after Init() in order to re-create the
    // contents of the desktoprc file in case the user or any other
    // program was messing around with the contents of the desktop
    // directory. Unused entries will get deleted, new entries will be
    // added for any new files...
    fileWindow->Save();

    fileWindow->ArrangeIcons();

    MapSubwindows();
    MoveResize(-100, -100, 90, 40);
    SetWMPosition(-100, -100);

    SetWindowName("fOX desktop");
    SetClassHints("Desktop", "Desktop");

    SetMWMHints(MWM_DECOR_ALL, MWM_FUNC_ALL, MWM_INPUT_MODELESS);

    Resize(GetDefaultSize());

    AddInput(FocusChangeMask);
}

OXDesktopMain::~OXDesktopMain() {
  delete fileWindow; 
 
  delete _newMenu;
  delete _sortMenu;

  delete objectMenu;
  delete sendToMenu;

  delete[] _dndTypeList;

  _dndManager->RemoveRootProxy();
}

void OXDesktopMain::_MakeMenus() {

  sendToMenu = new OXPopupMenu(_client->GetRoot());
  sendToMenu->AddEntry(new OHotString("Fax Recipient"),  21, _client->GetPicture("fax.t.xpm"));
  sendToMenu->AddEntry(new OHotString("Mail Recipient"), 22, _client->GetPicture("mail.t.xpm"));
  sendToMenu->AddEntry(new OHotString("My Briefcase"),   23, _client->GetPicture("briefcase.t.xpm"));

  objectMenu = new OXPopupMenu(_client->GetRoot());
  objectMenu->AddEntry(new OHotString("&Open"),    1501);
  //objectMenu->AddEntry(new OHotString("&Explore"), 1502);
  //objectMenu->AddEntry(new OHotString("&Find..."), 1503);
  objectMenu->AddSeparator();
  objectMenu->AddPopup(new OHotString("Se&nd To"), sendToMenu);
  objectMenu->AddSeparator();
  objectMenu->AddEntry(new OHotString("Cu&t"),   M_EDIT_CUT);
  objectMenu->AddEntry(new OHotString("&Copy"),  M_EDIT_COPY);
  objectMenu->AddEntry(new OHotString("&Paste"), M_EDIT_PASTE);
  objectMenu->AddSeparator();
  //objectMenu->AddEntry(new OHotString("Create &Shortcut"), M_FILE_NEWSHORTCUT);
  objectMenu->AddEntry(new OHotString("&Delete"), M_FILE_DELETE);
  objectMenu->AddEntry(new OHotString("Rena&me"), M_FILE_RENAME);
  objectMenu->AddEntry(new OHotString("Change &Icon..."), 888);
  objectMenu->AddSeparator();
  objectMenu->AddEntry(new OHotString("P&roperties..."), M_FILE_PROPS);
  objectMenu->SetDefaultEntry(1501);

  sendToMenu->Associate(this);
  objectMenu->Associate(this);

  _newMenu = new OXPopupMenu(_client->GetRoot());
  _newMenu->AddEntry(new OHotString("&Folder"),       M_FILE_NEWFOLDER);
  _newMenu->AddEntry(new OHotString("&Shortcut"),     M_FILE_NEWSHORTCUT);
  _newMenu->AddSeparator();
  _newMenu->AddEntry(new OHotString("Bitmap Image"),  113);
  _newMenu->AddEntry(new OHotString("Text Document"), 113);
  _newMenu->AddEntry(new OHotString("Briefcase"),     113);

  _newMenu->DisableEntry(M_FILE_NEWSHORTCUT);

  _sortMenu = new OXPopupMenu(_client->GetRoot());
  _sortMenu->AddEntry(new OHotString("By &Name"),      M_VIEW_ARRANGE_BYNAME);
  _sortMenu->AddEntry(new OHotString("By &Type"),      M_VIEW_ARRANGE_BYTYPE);
  _sortMenu->AddEntry(new OHotString("By &Size"),      M_VIEW_ARRANGE_BYSIZE);
  _sortMenu->AddEntry(new OHotString("By &Date"),      M_VIEW_ARRANGE_BYDATE);
  _sortMenu->AddSeparator();
  _sortMenu->AddEntry(new OHotString("Lin&e up Icons"), M_VIEW_LINEUP);
  _sortMenu->AddEntry(new OHotString("&Auto Arrange"), M_VIEW_ARRANGE_AUTO);
  _sortMenu->CheckEntry(M_VIEW_ARRANGE_AUTO);
  _sortMenu->RCheckEntry(M_VIEW_ARRANGE_BYNAME, 
                         M_VIEW_ARRANGE_BYNAME, M_VIEW_ARRANGE_BYDATE);

  _sortMenu->Associate(this);

}

int OXDesktopMain::_CreateDesktop(const char *path) {

  // make the path...

  if (MakePath(path, S_IRWXU)) return errno;

  // ...and create some useful objects

  if (chdir(path)) return errno;

  const char *home = getenv("HOME");
  if (home)
    symlink(home, "Home");

  return 0;
}

int OXDesktopMain::HandleFocusChange(XFocusChangeEvent *event) {
  if ((event->mode == NotifyNormal) &&
      (event->detail != NotifyPointer)) {
    if (event->type == FocusIn) {
    } else {
      fileWindow->UnselectAll();
    }
  }
  return True;
}

int OXDesktopMain::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  switch (msg->action) {

    case MSG_CLICK:
      switch (msg->type) {

        case MSG_MENU:
          switch (wmsg->id) {

            case M_FILE_CLOSE:
              delete this; // mainWindow;
              break;

            case M_VIEW_ARRANGE_BYNAME:
              _sortMenu->RCheckEntry(M_VIEW_ARRANGE_BYNAME,
                                     M_VIEW_ARRANGE_BYNAME, M_VIEW_ARRANGE_BYDATE);
              fileWindow->Sort(SORT_BY_NAME);
              break;

            case M_VIEW_ARRANGE_BYTYPE:
              _sortMenu->RCheckEntry(M_VIEW_ARRANGE_BYTYPE,
                                     M_VIEW_ARRANGE_BYNAME, M_VIEW_ARRANGE_BYDATE);
              fileWindow->Sort(SORT_BY_TYPE);
              break;

            case M_VIEW_ARRANGE_BYSIZE:
              _sortMenu->RCheckEntry(M_VIEW_ARRANGE_BYSIZE,
                                     M_VIEW_ARRANGE_BYNAME, M_VIEW_ARRANGE_BYDATE);
              fileWindow->Sort(SORT_BY_SIZE);
              break;

            case M_VIEW_ARRANGE_BYDATE:
              _sortMenu->RCheckEntry(M_VIEW_ARRANGE_BYDATE,
                                     M_VIEW_ARRANGE_BYNAME, M_VIEW_ARRANGE_BYDATE);
              fileWindow->Sort(SORT_BY_DATE);
              break;

            case M_VIEW_REFRESH:
              fileWindow->DisplayDirectory();
              break;

            case M_FILE_PROPS:
              if (fileWindow->NumSelected() == 1) {
                const OXDesktopIcon *f;
                void *iterator = NULL;

                f = fileWindow->GetNextSelected(&iterator);
                new OXPropertiesDialog(_client->GetRoot(), NULL,
                                       new OString(f->GetName()));
              } else {
                // group file properties: chmod only?
                XBell(GetDisplay(), 0);
              }
              break;

            default:
              XBell(GetDisplay(), 0);
              break;

          }
          break;

        default:
          break;

      }
      break;

    default:
      break;

  }

  return True;
}

OXFrame *OXDesktopMain::GetFrameFromPoint(int x, int y) {
  int x_root, y_root;
  Window child;

  XTranslateCoordinates(GetDisplay(), _id, _client->GetRoot()->GetId(),
                        x, y, &x_root, &y_root, &child);

  return fileWindow->GetFrameFromPoint(x_root, y_root);
}
