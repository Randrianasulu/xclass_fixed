/**************************************************************************

    This file is part of xclass.
    Copyright (C) 1998-2002 Mike McDonald, Hector Peraza.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#include <X11/keysym.h>

#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXComboBox.h>
#include <xclass/OString.h>

#include "icons/arrow-down.xpm"

const OPicture *OXComboBox::_bpic = NULL;


//-------------------------------------------------------------------

OXComboBox::OXComboBox(const OXWindow *p, char *name, int id,
                       unsigned int options, unsigned long back) :
  OXCompositeFrame(p, 100, 100, options, back) {

    _widgetID = id;
    _widgetType = "OXComboBox";
    _msgObject = p;

    if (!_bpic) {
      _bpic = _client->GetPicture("arrow-down.xpm", XCP_arrow_down);
      if (!_bpic) FatalError("OXComboBox: bad or missing pixmap");
    }

    _te = new OXTextEntry(this, new OTextBuffer(255), 100);
    _te->ChangeOptions(CHILD_FRAME);
    _te->Resize(_te->GetDefaultWidth()*15, _te->GetDefaultHeight());
    _te->Associate(this);

    _but = new OXScrollBarElt(this, _bpic, SB_WIDTH, SB_WIDTH, RAISED_FRAME);

    _l1 = new OLayoutHints(LHINTS_LEFT | LHINTS_EXPAND_Y | LHINTS_EXPAND_X);
    _l2 = new OLayoutHints(LHINTS_RIGHT | LHINTS_EXPAND_Y);

    AddFrame(_te, _l1);
    AddFrame(_but, _l2);

    _dd = new OXDDPopup(_client->GetRoot(), 100, 100, VERTICAL_FRAME);

    _lb = new OXListBox(_dd, _widgetID, CHILD_FRAME);
    _lb->Resize(100, 100);
    _lb->Associate(_dd);
    _lb->TakeFocus(False);
    _dd->Associate(this);

    _dd->AddFrame(_lb, _l1);

    _dd->MapSubwindows();
    _dd->Resize(_dd->GetDefaultSize());

    XGrabButton(GetDisplay(), AnyButton, AnyModifier, _id, False,
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                GrabModeAsync, GrabModeAsync, None, None);

    // We do not want to get explicitely the focus, instead we'll let
    // OXTextEntry to do the job for us. As a side effect, note that
    // SetFocusOwner must be called on combo->GetTextEntry(), should the
    // combo box have the default focus. (Perhaps we should derivate
    // OXComboBox directly from OXTextEntry?)

    TakeFocus(False);
}

OXComboBox::~OXComboBox() {
  delete _dd;
  delete _l1;
  delete _l2;
}

ODimension OXComboBox::GetDefaultSize() const {
  return ODimension (_w, _te->GetDefaultHeight() + _insets.t + _insets.b);
}

int OXComboBox::HandleButton(XButtonEvent *event) {
  if (event->subwindow == _te->GetId()) {
    return _te->HandleButton(event);
  } else if (event->subwindow == _but->GetId()){
    if (event->type == ButtonPress) {
      _but->SetState(BUTTON_DOWN);
    } else {
      _PopDown();
    }
  }
  return True;
}

void OXComboBox::_PopDown() {
  int ax, ay;
  Window wdummy;

  _but->SetState(BUTTON_UP);
  XTranslateCoordinates(GetDisplay(), _id, (_dd->GetParent())->GetId(),
                        0, _h, &ax, &ay, &wdummy);
  _dd->PlacePopup(ax, ay, _w-2, _dd->GetDefaultHeight());
}

void OXComboBox::_UpdateText(OXLBEntry *e) {
  if (e) {
    _te->Clear();
    _te->AddText(0, ((OXTextLBEntry *)e)->GetText()->GetString());
    _te->SelectAll();
  }
}

int OXComboBox::ProcessMessage(OMessage *msg) {
  OXLBEntry *e;
  OTextEntryMessage *temsg;
  OListBoxMessage *lbmsg;

  switch (msg->type) {
    case MSG_TEXTENTRY:
      temsg = (OTextEntryMessage *) msg;
      switch (temsg->action) {
        case MSG_TEXTCHANGED:
          switch (temsg->keysym) {
            case XK_Up:
              _lb->MoveSelectUp(GetSelectedEntry());
              e = _lb->GetSelectedEntry();
              _UpdateText(e);
              break;

            case XK_Down:
              _lb->MoveSelectDown(GetSelectedEntry());
              e = _lb->GetSelectedEntry();
              _UpdateText(e);
              break;

            case XK_F4:
              if (!_dd->IsDown()) _PopDown();
              break;

            case XK_Escape:
            case XK_Tab:
            //case XK_Execute:
            case XK_Return:
            case XK_KP_Enter:
              // TODO: transform this into an OComboBox message
              SendMessage(_msgObject, msg);
              break;
          }
          break;
      }
      break;

    case MSG_DDLISTBOX:
      lbmsg = (OListBoxMessage *) msg;
      switch (msg->action) {
        case MSG_CLICK:
          e = _lb->GetSelectedEntry();
          if (e && lbmsg->id) {
            _UpdateText(e);
            // TODO: transform this into an OComboBox message
            OListBoxMessage message(MSG_LISTBOX, MSG_CLICK,
                                    _widgetID, e->ID());
            SendMessage(_msgObject, &message);
          }
          break;
      }
      break;
  }

  return True;
}
