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

#ifndef __OIRCMESSAGE_H
#define __OIRCMESSAGE_H

#include <xclass/OString.h>
#include <xclass/OMessage.h>

#include "OTcpMessage.h"


#define IRC_NICKLIST		(MSG_USERMSG+10)

#define INCOMING_IRC_MSG	(MSG_USERMSG+22)
#define OUTGOING_IRC_MSG	(MSG_USERMSG+23)
#define INCOMING_CTCP_MSG	(MSG_USERMSG+24)
#define OUTGOING_CTCP_MSG	(MSG_USERMSG+25)

#define PRIVMSG			0
#define NOTICE			1
#define AUTH			2
#define ERROR			3
#define JOIN			4
#define LEAVE			5
#define MODE			6
#define TOPIC			7
#define TOPIC_SET		8
#define TOPIC_SETBY		9
#define NICK			10
#define NICKLIST		11
#define USER			12
#define KICK			13
#define QUIT			14
#define NAMES			15
#define WHO			16
#define SPECIAL			17
#define UNKNOWN			18
#define DCC			19
#define WALLOPS			20
#define INVITE			21


class OIrcMessage : public OTcpMessage {
public:
  OIrcMessage(int typ, int act,
              long p1, long p2, long p3, long p4,
              char *str1 = "", char *str2 = "",
              char *str3 = "", char *str4 = "") :
    OTcpMessage(typ, act, p1, p2, p3, p4, str1) {
      string2 = new OString(str2);
      string3 = new OString(str3);
      string4 = new OString(str4);
      printf("new OIrcMessage: %i, %i, %i, %i, %i, %i\n", 
             typ, act, p1, p2, p3, p4);
      printf("str1: \"%s\"\n", str1);
      printf("str2: \"%s\"\n", str2);
      printf("str3: \"%s\"\n", str3);
      printf("str4: \"%s\"\n", str4);
  }
  ~OIrcMessage() {
    delete string2;
    delete string3;
    delete string4;
  }

  OString *string2;
  OString *string3;
  OString *string4;
};

#endif  // __OIRCMESSAGE_H
