/**************************************************************************

    This file is part of xclass' desktop manager.
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

#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OString.h>
#include <xclass/OPicture.h>
#include <xclass/OTimer.h>
#include <xclass/ODNDmanager.h>


#define M_FILE_NEW		101
#define M_FILE_NEWFOLDER	    111
#define M_FILE_NEWSHORTCUT	    112
#define M_FILE_DELETE		102
#define M_FILE_RENAME		103
#define M_FILE_PROPS		104
#define M_FILE_CLOSE		105

#define M_EDIT_UNDO		201
#define M_EDIT_CUT		202
#define M_EDIT_COPY		203
#define M_EDIT_PASTE		204
#define M_EDIT_PASTESHORTCUT	205
#define M_EDIT_SELECTALL	206
#define M_EDIT_INVSELECTION	207

#define M_VIEW_ARRANGE_BYNAME	    311
#define M_VIEW_ARRANGE_BYTYPE	    312
#define M_VIEW_ARRANGE_BYSIZE	    313
#define M_VIEW_ARRANGE_BYDATE	    314
#define M_VIEW_ARRANGE_AUTO	    315
#define M_VIEW_LINEUP           307
#define M_VIEW_REFRESH          308

#define M_ROOT_LINEUP           401
#define M_ROOT_PROPS            402


class OXPopupMenu;
class OXDesktopContainer;


//---------------------------------------------------------------------

class OXDesktopMain : public OXMainFrame {
public:
  OXDesktopMain(const OXWindow *p, int w, int h);
  virtual ~OXDesktopMain();

  virtual int HandleConfigureNotify(XConfigureEvent *event);
  virtual int HandleFocusChange(XFocusChangeEvent *event);
  virtual int HandleTimer(OTimer *t);
  virtual int ProcessMessage(OMessage *msg);

  virtual OXFrame *GetFrameFromPoint(int x, int y);  

protected:
  int  _CreateDesktop(const char *path);
  void _MakeMenus();

  OTimer *_t;
  OXDesktopContainer *_container;
  OXPopupMenu *_newMenu, *_sortMenu, *_rootMenu;
  Atom *_dndTypeList;
};
