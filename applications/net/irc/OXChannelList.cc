/**************************************************************************

    This file is part of foxirc.
    Copyright (C) 2000-2002, Hector Peraza.

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
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <xclass/utils.h>

#include "OIrcMessage.h"
#include "OXChannelList.h"
#include "OXIrc.h"


//----------------------------------------------------------------------

OXChannelList::OXChannelList(const OXWindow *p, const OXWindow *main,
                             OXIrc *irc, int w, int h) :
  OXTransientFrame(p, main, w, h) {

  _irc = irc;

  //---- list view

  _listView = new OXListView(this, 10, 10, 1);

  _listView->AddColumn(new OString("Channel"), 0, TEXT_LEFT);
  _listView->AddColumn(new OString("Users"), 1);
  _listView->AddColumn(new OString("Title"), 2, TEXT_LEFT);
  _listView->AddColumn(new OString(""), 3);  // end dummy

  OLayoutHints *layout = new OLayoutHints(LHINTS_EXPAND_ALL);

  AddFrame(_listView, layout);
  _listView->SetViewMode(LV_DETAILS);
  _listView->Associate(this);

  SetWindowName("Channel list");
  SetClassHints("fOXIrc", "dialog");

  MapSubwindows();
  Resize(560, 320);
  Layout();
  MapWindow();
}

OXChannelList::~OXChannelList() {
  ClearList();
}

int OXChannelList::CloseWindow() {
  _irc->ChannelListClosed();
  return OXTransientFrame::CloseWindow();
}

int OXChannelList::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;
  OItemViewMessage *vmsg = (OItemViewMessage *) msg;

  switch (msg->type) {
    case MSG_BUTTON:
      switch (msg->action) {

        case MSG_CLICK:
          switch (wmsg->id) {

            default:
              break;

          }

        default:
          break;

      }
      break;

    case MSG_LISTVIEW:
      switch (msg->action) {
        case MSG_DBLCLICK:
          if (vmsg->button == Button1) {
            if (_listView->NumSelected() == 1) {
              const OListViewItem *i;
              vector<OItem *> items;

              items = _listView->GetSelectedItems();
              i = (OListViewItem *) items[0];

              _irc->JoinChannel(i->GetName()->GetString());
            }
          }
          break;

        case MSG_SELCHANGED:
          // enable/disable join buttons...
          break;
      }
      break;

    default:
      break;

  }

  return True;
}

//----------------------------------------------------------------------

void OXChannelList::ClearList() {
  _listView->Clear();
}

void OXChannelList::AddChannel(const char *name, int nusers,
                               const char *title) {
  OListViewItem *item;
  vector<OString *> names;
  char tmp[20];

  names.clear();
  names.push_back(new OString(name));
  sprintf(tmp, "%4d", nusers);
  names.push_back(new OString(tmp));
  names.push_back(new OString(title));

  item = new OListViewItem(_listView, _listView->GetNumberOfItems() + 1,
                           NULL, NULL, names, LV_DETAILS);
  _listView->AddItem(item);
  _listView->Layout();
}
