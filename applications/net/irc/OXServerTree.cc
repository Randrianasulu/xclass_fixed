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
#include "OXServerTree.h"
#include "OXIrc.h"


//----------------------------------------------------------------------

OServerLink::OServerLink(const char *srvname, const char *conn,
                         const char *msg, int hopcnt) {
  serverName = StrDup(srvname);
  connectedTo = StrDup(conn);
  serverMsg = StrDup(msg);
  hop = hopcnt;
}

OServerLink::~OServerLink() {
  delete[] serverName;
  delete[] connectedTo;
  delete[] serverMsg;
}

//----------------------------------------------------------------------

OXServerTree::OXServerTree(const OXWindow *p, const OXWindow *main,
                           OXIrc *irc, int w, int h) :
  OXTransientFrame(p, main, w, h) {

  _irc = irc;
  _links.clear();
  _clearPending = False;

  //---- list tree

  _listTree = new OXListTree(this, 10, 10, 1, SUNKEN_FRAME | DOUBLE_BORDER);
  _listTree->Associate(this);

  OLayoutHints *layout = new OLayoutHints(LHINTS_EXPAND_ALL);

  AddFrame(_listTree, layout);

  SetWindowName("Server Links");
  SetClassHints("fOXIrc", "dialog");

  MapSubwindows();
  Resize(560, 320);
  Layout();
  MapWindow();
}

OXServerTree::~OXServerTree() {
  Clear();
}

int OXServerTree::CloseWindow() {
  _irc->ServerTreeClosed();
  return OXTransientFrame::CloseWindow();
}

int OXServerTree::ProcessMessage(OMessage *msg) {
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

    case MSG_LISTTREE:
      switch (msg->action) {
        case MSG_CLICK:
          break;

        case MSG_SELCHANGED:
          // enable/disable connect buttons...
          break;
      }
      break;

    default:
      break;

  }

  return True;
}

//----------------------------------------------------------------------

void OXServerTree::Clear() {
  OListTreeItem *next, *item = _listTree->GetFirstItem();
  while (item) {
    next = item->nextsibling;
    _listTree->DeleteChildren(item);
    _listTree->DeleteItem(item);
    item = next;
  }
  for (int i = 0; i < _links.size(); ++i) delete _links[i];
  _links.clear();
  _clearPending = False;
}

void OXServerTree::AddLink(OServerLink *link) {
  if (_clearPending) Clear();
  _links.push_back(link);
}

void OXServerTree::BuildTree() {
  int i, hopcnt, found, total;
  OListTreeItem *e;

  total = 0;
  hopcnt = 0;
  found = True;

  while (found) {
    found = False;
    for (i = 0; i < _links.size(); ++i) {
      if (_links[i]->hop == hopcnt) {
        found = True;
        ++total;
        if (hopcnt == 0) {   // this is a root server
          e = _listTree->AddItem(NULL, _links[i]->serverName);
          e->open = True;
        } else {
          e = _FindServer(NULL, _links[i]->connectedTo, hopcnt - 1);
          if (e) {
            e = _listTree->AddItem(e, _links[i]->serverName);
            e->open = True;
          }
        }
      }
    }
    ++hopcnt;
  }

  printf("size = %d, total = %d\n", _links.size(), total);

  _listTree->Layout();
  _clearPending = True;
}

OListTreeItem *OXServerTree::_FindServer(OListTreeItem *root,
                                         const char *name, int hopcnt) {
  OListTreeItem *item, *ret;

  if (!root) root = _listTree->GetFirstItem();

  for (item = root; item; item = item->nextsibling) {
    if ((strcmp(item->text, name) == 0) && (hopcnt == 0)) return item;
    if (item->firstchild) {
      ret = _FindServer(item->firstchild, name, hopcnt-1);
      if (ret) return ret;
    }
  }

  return NULL;
}
