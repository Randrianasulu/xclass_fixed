/**************************************************************************

    This file is part of Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#ifndef __OCOMPONENT_H
#define __OCOMPONENT_H

#include <X11/Xlib.h>
#include <xclass/OMessage.h>


// xclass generic object, objects derivated from this can 
// communicate with each other.
//---------------------------------------------------------------

class OTimer;
class OFileHandler;
class OIdleHandler;
class OXClient;

class OComponent : public OBaseObject {
protected:
  OXClient *_client;           // pointer to the X client

  friend class OXClient;
  friend class OTimer;
  friend class OFileHandler;
  friend class OIdleHandler;

public:
  OComponent() : _client(0) {}

  virtual void SendMessage(const OComponent *obj, OMessage *msg) {
    if (obj) ((OComponent*)obj)->ProcessMessage(msg);
  }
  virtual int ProcessMessage(OMessage *msg) { return False; }

  virtual int HandleTimer(OTimer *) { return False; }
  virtual int HandleFileEvent(OFileHandler *, unsigned int) { return False; }
  virtual int HandleIdleEvent(OIdleHandler *) { return False; }
};


#endif  // __OCOMPONENT_H
