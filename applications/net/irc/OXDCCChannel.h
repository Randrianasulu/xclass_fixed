#ifndef __OXDCCCHANNEL_H
#define __OXDCCCHANNEL_H

#include <stdlib.h>
#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXTransientFrame.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXMenu.h>
#include <xclass/OXStatusBar.h>
#include <xclass/OString.h>

#include "OXViewDoc.h"
#include "OXNameList.h"
#include "OXIrc.h"
#include "OXChannel.h"


//----------------------------------------------------------------------

class OXDCCChannel : public OXChannel {
public:
  OXDCCChannel(const OXWindow *p, const OXWindow *main, OXIrc *s,
               const char *ch);
  virtual ~OXDCCChannel();

  virtual int ProcessMessage(OMessage *msg);
  virtual int HandleFileEvent(OFileHandler *fh, unsigned int mask);

  int  Connect(const char *server, int port);
  int  Listen(unsigned long *host, unsigned short *port);
  void Disconnect(int log = True);
  int  ProcessDCCCommand(char *cmd);

protected:
  void _UpdateStatusBar();

  OXMenuBar *_menubar;
  OXPopupMenu *_menuchannel, *_menumode, *_menulog, *_menuedit,
              *_menuview, *_menuhelp, *_nick_ctcp, *_nick_dcc,
              *_nick_actions, *_nick_ignore;

  OXStatusBar *_statusBar;

  OTcp *_dccServer;
  OFileHandler *_fl;
  char *_serverName;
  int _port;
  bool _connected, _serverSocket;
};


#endif  // __OXDCCCHANNEL_H
