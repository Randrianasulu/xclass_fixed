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

#ifndef __OTCPMESSAGE_H
#define __OTCPMESSAGE_H

#include <xclass/OString.h>
#include <xclass/OMessage.h>


#define INCOMING_TCP_MSG	(MSG_USERMSG+20)
#define OUTGOING_TCP_MSG	(MSG_USERMSG+21)
#define BROKEN_PIPE		(MSG_USERMSG+25)

//-------------------------------------------------------------------------

class OTcpMessage : public OMessage {
public:
  OTcpMessage(int typ, int act,
              long p1, long p2, long p3, long p4, char *str1 = "") :
    OMessage(typ, act) {
      parm1 = p1;
      parm2 = p2;
      parm3 = p3;
      parm4 = p4;
      string1 = new OString(str1);
      printf("new OTcpMessage: %i, %i, %i, %i, %i, %i\n",
                               typ, act, p1, p2, p3, p4);
      printf("str1: \"%s\"\n", str1);
  }
  ~OTcpMessage() { delete string1; }
	
  long parm1, parm2, parm3, parm4;
  OString *string1;
};

#endif  // __OTCPMESSAGE_H
