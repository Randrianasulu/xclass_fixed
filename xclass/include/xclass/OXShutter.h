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

#ifndef __OXSHUTTER_H
#define __OXSHUTTER_H

#include <X11/X.h>
#include <X11/Xlib.h>

#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXCompositeFrame.h>
#include <xclass/OXCanvas.h>
#include <xclass/OXWidget.h>
#include <xclass/OXButton.h>
#include <xclass/OTimer.h>


//---------------------------------------------------------------------
// Shutter item object

class OXShutterItem : public OXVerticalFrame, public OXWidget {
public:
  OXShutterItem(const OXWindow *p, OString *s = NULL, int ID = -1,
                unsigned int options = 0);
  virtual ~OXShutterItem();

  OXFrame* GetContainer() const { return _canvas->GetContainer(); }

  friend class OXShutter;  

protected:
  OXButton *_button;
  OXCanvas *_canvas;
  OXFrame  *_container;
  OLayoutHints *_l1, *_l2;
};


//---------------------------------------------------------------------
// Shutter widget

class OXShutter : public OXCompositeFrame {
public:
  OXShutter(const OXWindow *p, unsigned int options = SUNKEN_FRAME);
  virtual ~OXShutter();

  virtual void AddItem(OXShutterItem *item);
  virtual int  HandleTimer(OTimer *t);
  virtual void Layout();

  virtual int ProcessMessage(OMessage *msg);

  friend class OXShutterItem;  

protected:
  OLayoutHints *_lh;
  OTimer *_timer;                   // Timer for animation
  OXShutterItem *_selectedItem;     // Item currently open
  OXShutterItem *_closingItem;      // Item closing down
  int _heightIncrement;             // Height delta
  int _closingHeight;               // Closing items current height
  int _closingHadScrollbar;         // Closing item had a scroll bar
};


#endif  // __OXSHUTTER_H
