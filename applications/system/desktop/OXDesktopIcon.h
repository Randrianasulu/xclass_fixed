/**************************************************************************

    This file is part of a xclass desktop manager for fvwm95.
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

#ifndef __OXDESKTOPICON_H
#define __OXDESKTOPICON_H

#include <X11/Xlib.h>

#include <xclass/OXFrame.h>
#include <xclass/OString.h>
#include <xclass/OPicture.h>
#include <xclass/OMessage.h>


class OXFont;
class OXGC;
class OSelectedPicture;


#define MSG_DESKTOPICON      (MSG_USERMSG+1234)

#define MSG_MOVED            10
#define MSG_DROP             11
#define MSG_OPEN             12

class ODesktopIconMessage : public OWidgetMessage {
public:
  ODesktopIconMessage(int typ, int act, int wid) :
    OWidgetMessage(typ, act, wid) {}

public:
};


//-------------------------------------------------------------------

class OXDesktopIcon : public OXFrame {
protected:
  static Cursor _defaultCursor;
  static unsigned int _selPixel;
  static OXGC *_defaultGC;
  static const OXFont *_defaultFont;

public:
  OXDesktopIcon(const OXWindow *p, const OPicture *pic, const OPicture *lpic,
                OString *name, int type, unsigned long size,
                unsigned int options = CHILD_FRAME,
                unsigned long back = _defaultDocumentBackground);
  virtual ~OXDesktopIcon();

  void  Activate(int a);
  int   IsActive() const { return _active; }
  int   IsSymLink() const { return _is_link; }
  const OString *GetName() const { return _name; }
  const OPicture *GetPicture() const { return _pic; }
  int   GetType() const { return _type; }
  int   GetSize() const { return _size; }

  virtual ODimension GetDefaultSize() const;

  virtual int HandleButton(XButtonEvent *event);
  virtual int HandleMotion(XMotionEvent *event);
  virtual int HandleDoubleClick(XButtonEvent *event);
  virtual int HandleClientMessage(XClientMessageEvent *event);

  virtual int HandleSelection(XSelectionEvent *event);
  virtual int HandleSelectionRequest(XSelectionRequestEvent *event);

  virtual Atom HandleDNDenter(Atom *typelist);
  virtual int  HandleDNDleave();
  virtual Atom HandleDNDposition(int x, int y, Atom action, int xr, int yr);
  virtual int  HandleDNDdrop(ODNDdata *DNDdata);
  virtual int  HandleDNDfinished();

  virtual ODNDdata *GetDNDdata(Atom dataType);

  virtual void Layout();

  void SetFileWindow(OXFrame *f) { _fw = f; }
  void InitPos();
  void PlaceIcon() { PlaceIcon(bx, by); }
  void PlaceIcon(int x, int y);

protected:
  virtual void _DoRedraw();
  
  void _SetDragPixmap();
  
  OString *_name;
  int _active, _last_state, _dragging, _tw, _th, _ta, _type, _is_link;
  unsigned long _size;
  int x0, y0, bx, by, _button, _bdown;
  OXFrame *_fw;
  const OPicture *_pic, *_lpic;
  OXGC *_normGC;
  const OXFont *_font;
  OSelectedPicture *_selpic;

#if 1
  ODNDmanager *_dndManager;
#endif
};


#endif  // __OXDESKTOPICON_H
