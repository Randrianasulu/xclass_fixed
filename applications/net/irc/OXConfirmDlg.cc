/**************************************************************************

    This file is part of foxirc, a cool irc client.
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
  
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "OXConfirmDlg.h"


//---------------------------------------------------------------------

OXConfirmDlg::OXConfirmDlg(const OXWindow *p, const OXWindow *main,
                           OString *title, OString *text, OString *msg,
                           int *ret_code, unsigned long options) :
  OXTransientFrame(p, main, 10, 10, options) {

    int ax, ay, width = 0, height = 0;
    unsigned int root_w, root_h, dummy;
    Window wdummy;

    _ret_code = ret_code;
    _msg = msg;

    L1 = new OLayoutHints(LHINTS_EXPAND_X | LHINTS_TOP, 3, 3, 5, 5);

    //--- create the label for the top text

    L3 = new OLayoutHints(LHINTS_CENTER_X | LHINTS_TOP, 5, 5, 5, 0);
    AddFrame(new OXLabel(this, text), L3);

    //--- create the frame for the message entry box

    OXCompositeFrame *hframe = new OXHorizontalFrame(this, 0, 0, 0);
    msge = new OXTextEntry(hframe, new OTextBuffer(100));
    msge->Resize(250, msge->GetDefaultHeight());

    L4 = new OLayoutHints(LHINTS_CENTER_Y | LHINTS_LEFT, 0, 5, 5, 5);
    L5 = new OLayoutHints(LHINTS_RIGHT | LHINTS_EXPAND_X | LHINTS_CENTER_Y);

    hframe->AddFrame(new OXLabel(hframe, new OString("Message:")), L4);
    hframe->AddFrame(msge, L5);

    AddFrame(hframe, L1);
    AddFrame(new OXHorizontal3dLine(this), L1);

    //--- create the buttons and button frame

    OXCompositeFrame *ButtonFrame = new OXHorizontalFrame(this,
                                                    60, 20, FIXED_WIDTH);

    Yes = new OXTextButton(ButtonFrame, new OHotString("&Yes"), ID_YES);
    Yes->SetDefault();
    Yes->Associate(this);
    ButtonFrame->AddFrame(Yes, L1);
    width = max(width, Yes->GetDefaultWidth());

    No = new OXTextButton(ButtonFrame, new OHotString("&No"), ID_NO);
    No->Associate(this);
    ButtonFrame->AddFrame(No, L1);
    width = max(width, No->GetDefaultWidth());

    //--- place buttons at the bottom

    L2 = new OLayoutHints(LHINTS_BOTTOM | LHINTS_CENTER_X);
    AddFrame(ButtonFrame, L2);

    //--- Keep the buttons centered and with the same width

    ButtonFrame->Resize((width + 20) * 2, GetDefaultHeight());

    //_focusMgr->SetFocusOwner(msge);
    SetFocusOwner(msge);
    MapSubwindows();
    
    width  = GetDefaultWidth();
    height = GetDefaultHeight();

    Resize(width, height);

    //--- position relative to the parent's window

    if (main) {
      XTranslateCoordinates(GetDisplay(),
                            main->GetId(), GetParent()->GetId(),
                            (((OXFrame *) main)->GetWidth() - _w) >> 1,
                            (((OXFrame *) main)->GetHeight() - _h) >> 1,
                            &ax, &ay, &wdummy);
    } else {
      XGetGeometry(GetDisplay(), _client->GetRoot()->GetId(), &wdummy,
                   &ax, &ay, &root_w, &root_h, &dummy, &dummy);
      ax = (root_w - _w) >> 1;
      ay = (root_h - _h) >> 1;
    }

    Move(ax, ay);
    SetWMPosition(ax, ay);

    //---- make the dialog box non-resizable

    SetWMSize(width, height);
    SetWMSizeHints(width, height, width, height, 0, 0);

    //---- set names

    SetWindowName((char *) title->GetString());
    SetIconName((char *) title->GetString());
    SetClassHints("MsgBox", "MsgBox");

    SetMWMHints(MWM_DECOR_ALL | MWM_DECOR_RESIZEH | MWM_DECOR_MAXIMIZE |
                                MWM_DECOR_MINIMIZE | MWM_DECOR_MENU,
                MWM_FUNC_ALL | MWM_FUNC_RESIZE | MWM_FUNC_MAXIMIZE |
                               MWM_FUNC_MINIMIZE,
                MWM_INPUT_MODELESS);

    MapWindow();
    _client->WaitFor(this);
}

OXConfirmDlg::~OXConfirmDlg() {
  delete L1;
  delete L2;
  delete L3;
  delete L4;
  delete L5;
}

int OXConfirmDlg::CloseWindow() {
  if (_ret_code) *_ret_code = ID_NO;
  return OXTransientFrame::CloseWindow();
}

int OXConfirmDlg::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  switch(msg->type) {
  case MSG_BUTTON:
    switch(msg->action) {
    case MSG_CLICK:
      if (_ret_code) *_ret_code = wmsg->id;
      if (_msg) _msg->Append(msge->GetString());
      delete this;
      break;

    default:
      break;
    }
    break;
  }
  return True;
}
