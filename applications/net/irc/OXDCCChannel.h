#ifndef __OXDCCCHANNEL_H
#define __OXDCCCHANNEL_H

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


//----------------------------------------------------------------------

class OXDCCChannel : public OXChannel {
public:
  OXDCCChannel(const OXWindow *p, const OXWindow *main, OXIrc *s,
               const char *ch);
  virtual ~OXDCCChannel();

  int  Connect(char *server, int port);
  int  Listen(unsigned long *host, unsigned short *port);
  void CloseUp();

  virtual int ProcessMessage(OMessage *msg);
  virtual int HandleFileEvent(OFileHandler *fh, unsigned int mask);
          int ProcessDCCCommand(char *cmd);

protected:
//  OXPopupMenu *_menuchannel, *_menulog;
//  OXMenuBar *_menubar;
//  OXTextEntry *_User, *_Server, *_Info;
//  OXLabel *_UserLabel, *_ServerLabel, *_InfoLabel;
//  OXCheckBox *_dcc, *_chat;
  OTcp *_dccServer;
  OFileHandler *_fl;
  bool _coned, _serverSocket;
};

#endif  // __OXDCCCHANNEL_H
