#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <errno.h>

#include "OXDCCFile.h"
#include "IRCcodes.h"
#include "OIrcMessage.h"
#include "OXDCCChannel.h"
#include "OXIrc.h"
#include "OXConfirmDlg.h"


//----------------------------------------------------------------------

extern OLayoutHints *topleftlayout;
extern OLayoutHints *topexpandxlayout;
extern OLayoutHints *topexpandxlayout0;
extern OLayoutHints *topexpandxlayout1;
extern OLayoutHints *topexpandxlayout2;
extern OLayoutHints *topexpandxlayout3;
extern OLayoutHints *leftexpandylayout;
extern OLayoutHints *leftcenterylayout;
extern OLayoutHints *expandxexpandylayout;
extern OLayoutHints *menubarlayout;
extern OLayoutHints *menuitemlayout;

//extern OXPopupMenu *_nick_actions, *_nick_ctcp;

extern OSettings  *foxircSettings;

extern char *filetypes[];

//----------------------------------------------------------------------

OXDCCChannel::OXDCCChannel(const OXWindow *p, const OXWindow *main,
                           OXIrc *s, const char *ch) :
  OXChannel(p, main, s, ch, true) {
    char wname[100];

//    sprintf(wname, "DCC Chat with %s", &_name[1]);
    sprintf(wname, "=%s DCC Chat", &_name[1]);
    SetWindowName(wname);

    _dccServer = new OTcp(); 
    _dccServer->Associate(this);
    _coned = False;
    _serverSocket = False;
    _fl = NULL;
}

OXDCCChannel::~OXDCCChannel() {
  if (_coned) CloseUp();
  if (_fl) delete _fl;
  if (_dccServer) delete _dccServer;
}

int OXDCCChannel::Connect(char *server, int port) {
  int ret = _dccServer->Connect(strtoul(server, NULL, 10), port, True);
  if (_fl) delete _fl;
  _fl = new OFileHandler(this, _dccServer->GetFD(),
                               XCM_WRITABLE | XCM_EXCEPTION);
  _serverSocket = False;
  return ret;
}

int OXDCCChannel::Listen(unsigned long *host, unsigned short *port) {
  int ret = _dccServer->Listen(host, port, _server->GetOIrc()->GetOTcp());

  if (ret < 0) return ret;
  if (_fl) delete _fl;
  _fl = new OFileHandler(this, _dccServer->GetFD(), XCM_READABLE);
  _serverSocket = True;
  Log("Waiting for connection...", 4);

  return ret;
}

void OXDCCChannel::CloseUp() {
  if (_coned) {
    _dccServer->Close();
    if (_fl) delete _fl;
    _fl = NULL;
    _coned = False;
  }
}

int OXDCCChannel::HandleFileEvent(OFileHandler *fh, unsigned int mask) {
  if (fh != _fl) {
    return False;
  } else {
    if (_serverSocket) {
      if (_fl) { delete _fl; _fl = NULL; }
      OTcp *newconn = new OTcp();
      int ret = newconn->Accept(_dccServer->GetFD());  // write this better...
      delete _dccServer;  // we do not have to continue
                          // listening on that port...
      _dccServer = newconn;
      _dccServer->Associate(this);
      if (ret >= 0) {
        char s[256];

        _fl = new OFileHandler(this, _dccServer->GetFD(), XCM_READABLE);
        sprintf(s, "Connection to %s:%d established",
                    _dccServer->GetAddress(), _dccServer->GetPort());
        Log(s, 5);
        _coned = True;
      } else {
        Log("Connection failed", 5);
      }
      _serverSocket = False;

    } else {
      switch (mask) {
        case XCM_WRITABLE:
          if (!_coned) {
            _coned = True;
            Log("Connection established", 4);
            if (_fl) delete _fl;
            _fl = new OFileHandler(this, _dccServer->GetFD(), XCM_READABLE);
          }
          break;

        case XCM_READABLE:
          _dccServer->Receive();
          break;
      }
    }
  }

  return True;
}

int OXDCCChannel::ProcessMessage(OMessage *msg) {
  char char1[TCP_BUFFER_LENGTH], 
       char3[TCP_BUFFER_LENGTH],
       char4[TCP_BUFFER_LENGTH],
       m[TCP_BUFFER_LENGTH];
  int  n;

  switch(msg->type) {
    case MSG_TEXTENTRY:
    {
      OTextEntryMessage *tmsg = (OTextEntryMessage *) msg;
      switch(msg->action) {
        case MSG_TEXTCHANGED:
          switch(tmsg->keysym) {
            case XK_Return:
	      sprintf(char1, "%s\n", _sayentry->GetString());
	      printf("DCC Sending Message: %s\n", char1);
              if (strlen(char1) > 0) {
                if (_coned) {
		 if(!ProcessDCCCommand(char1)){
                  OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
                  SendMessage(_dccServer, &message);
                  Say(_server->_nick, char1, DCC);
		  }
                }
	      }
	      _sayentry->Clear();
              break;
          }
          break;
      }
      }
      break;

    case INCOMING_TCP_MSG:
      {
      OTcpMessage *tmsg = (OTcpMessage *)msg;
      printf("DCC Recieve Message :%s\n", tmsg->string1->GetString());
      Say(_name, (char *)tmsg->string1->GetString(), DCC);
      }
      break;

    case BROKEN_PIPE:
      Log("Remote end closed connection.", 5);
      CloseUp();
      break;
  }

  return True;
}

int OXDCCChannel::ProcessDCCCommand(char *cmd) {
  char char1[TCP_BUFFER_LENGTH],
       char2[TCP_BUFFER_LENGTH];
  int cmdsize;
  
  if (!cmd || *cmd != '/') return False;
  cmdsize = strlen(cmd);
  if (cmd[--cmdsize] == '\n') cmd[cmdsize] = 0;

  if (!strncmp(cmd, "/me ", 4)) {
    printf("Size of me is %d\n", cmdsize);
    sprintf(char1, "\1ACTION %s\1\n", &cmd[4]);
    {
      OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
      SendMessage(_dccServer, &message);
    }
    Say(_server->_nick, char1, PRIVMSG);
  } else {
    sprintf(char1, "Unknown Command %s\n", &cmd[1]);
    Log(char1, 5);
  }

  return True;
}
