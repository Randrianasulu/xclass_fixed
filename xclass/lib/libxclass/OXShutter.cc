/**************************************************************************

    This file is part of xclass.

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

#include <xclass/OXTextButton.h>
#include <xclass/OXShutter.h>

  
//--------------------------------------------------------------------

OXShutter::OXShutter(const OXWindow* p, unsigned int options) :
  OXCompositeFrame(p, 10, 10, options) {

    _selectedItem = NULL;
    _closingItem  = NULL;
    _heightIncrement = 1;
    _closingHeight = 0;
    _closingHadScrollbar = False;
    _timer = NULL;
    _lh = new OLayoutHints(LHINTS_EXPAND_X | LHINTS_EXPAND_Y);
}

OXShutter::~OXShutter() {
  if (_timer) delete _timer;
  delete _lh;
}

void OXShutter::AddItem(OXShutterItem *item) {
//  AddFrame(item, NULL);
  AddFrame(item, _lh);
}

int OXShutter::ProcessMessage(OMessage *msg) {
  register SListFrameElt *ptr;
  OXShutterItem *child, *item = NULL;

  if (!_flist) return 1;

  if (msg->type != MSG_BUTTON) return 0;
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  for (ptr=_flist; ptr != NULL; ptr=ptr->next) {
    child = (OXShutterItem *) ptr->frame;
    if (wmsg->id == child->WidgetID()) {
      item = child;
      break;
    }
  }

  if (!item) return 0;

  if (!_selectedItem) _selectedItem = (OXShutterItem*) _flist->frame;
  if (_selectedItem == item) return 1;

  _heightIncrement = 1;
  _closingItem = _selectedItem;
  _closingHeight = _closingItem->GetHeight();
  _closingHeight -= _closingItem->_button->GetDefaultHeight();
  _selectedItem = item;
  _timer = new OTimer(this, 6); //10);

  return 1;
}


//--- Shutter Item Animation

int OXShutter::HandleTimer(OTimer *t) {
  if (!_closingItem) return 0;
  _closingHeight -= _heightIncrement;
  _heightIncrement += 5;
  if (_closingHeight > 0) {
    _timer = new OTimer(this, 6); //10);
  } else {
    _closingItem = NULL;
    _closingHeight = 0;
    _timer = NULL;
  }
  Layout();

  return 1;   
}

void OXShutter::Layout() {
  register SListFrameElt *ptr;
  OXShutterItem *child;
  int y, bh, exh;

  if (!_flist) return;

  if (_selectedItem == NULL) _selectedItem = (OXShutterItem*) _flist->frame;

  exh = _h - (_bw << 1);
  for (ptr=_flist; ptr != NULL; ptr=ptr->next) {
    child = (OXShutterItem *) ptr->frame;
    bh = child->_button->GetDefaultHeight();
    exh -= bh;
  }

  y = _bw;
  for (ptr=_flist; ptr != NULL; ptr=ptr->next) {
    child = (OXShutterItem *) ptr->frame;
    bh = child->_button->GetDefaultHeight();
    if (child == _selectedItem) {
      if (_closingItem)
        child->_canvas->SetScrolling(CANVAS_NO_SCROLL);
      else
        child->_canvas->SetScrolling(CANVAS_SCROLL_VERTICAL);
      child->ShowFrame(child->_canvas);
      child->MoveResize(_bw, y, _w - (_bw << 1), exh - _closingHeight + bh);
      y += exh - _closingHeight + bh;
    } else if (child == _closingItem) {
      child->_canvas->SetScrolling(CANVAS_NO_SCROLL);
      child->MoveResize(_bw, y, _w - (_bw << 1), _closingHeight + bh);
      y += _closingHeight + bh;
    } else {
      child->MoveResize(_bw, y, _w - (_bw << 1), bh);
      child->HideFrame(child->_canvas);
      y += bh;
    }
  }
}


//--------------------------------------------------------------------

OXShutterItem::OXShutterItem(const OXWindow *p, OString *s, int ID,
                             unsigned int options) :
  OXVerticalFrame (p, 10, 10, options),
  OXWidget (ID, "OXShutterItem") {

  _button = new OXTextButton(this, s, ID);
  _canvas = new OXCanvas(this, 10, 10, CHILD_FRAME);
  _container = new OXVerticalFrame(_canvas->GetViewPort(), 10, 10, OWN_BKGND);
  _canvas->SetContainer(_container);
  _container->SetBackgroundColor(_client->GetShadow(_defaultFrameBackground)); 

  AddFrame(_button, _l1 = new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X));
  AddFrame(_canvas, _l2 = new OLayoutHints(LHINTS_EXPAND_Y | LHINTS_EXPAND_X));

  _button->Associate((OXFrame *) p);
}

OXShutterItem::~OXShutterItem() {
  delete _l1;
  delete _l2;
}
