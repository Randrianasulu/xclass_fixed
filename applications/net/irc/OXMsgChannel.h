#ifndef __OXMSGCHANNEL_H
#define __OXMSGCHANNEL_H

#include <stdlib.h>
#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXTransientFrame.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXSlider.h>
#include <xclass/OX3dLines.h>
#include <xclass/OString.h>

#include "OXTextView.h"
#include "OXNameList.h"
#include "OXIrc.h"
#include "TDList.h"
#include "OXChannel.h"


//---------------------------------------------------------------------

class OXMsgChannel : public OXChannel {
public:
  OXMsgChannel(const OXWindow *p, const OXWindow *main, OXIrc *s,
               const char *ch);
  virtual ~OXMsgChannel() {};

protected:
//  OXPopupMenu *_menuchannel, *_menulog;
//  OXMenuBar *_menubar;
//  OXTextEntry *_User, *_Server, *_Info;
//  OXLabel *_UserLabel, *_ServerLabel, *_InfoLabel;
//  OXCheckBox *_dcc, *_chat;
};


#endif  // __OXMSGCHANNEL_H
