#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <X11/keysym.h>

#include "IRCcodes.h"
#include "OIrcMessage.h"
#include "OXChannel.h"
#include "OXIrc.h"

#include "OXDCCFile.h"
#include "OXDCCChannel.h" //////

#include "versiondef.h"

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

extern OSettings *foxircSettings;

extern char *filetypes[];

//----------------------------------------------------------------------

OXChannel::OXChannel(const OXWindow *p, const OXWindow *main, OXIrc *s,
                     const char *ch, int init) :
  OXTransientFrame(p, main, 1, 1) {
  char wname[100];

  _server = s;
  _next = _prev = NULL;
  _name = StrDup(ch);

  _InitHistory();
  
  _cinfo = foxircSettings->FindChannel(_name);
  if (!_cinfo) _cinfo = new OChannelInfo();
  _cinfo->name = StrDup(_name);

  if (init) {
    SetLayoutManager(new OVerticalLayout(this));
    _AddView();

    //  if (_name[0] == '#')
    //    Resize(550, 300);
    //  else
    //    Resize(430, 300);
    if (_name[0] == '#')
      Resize(610, 360);
    else
      Resize(520, 360);

    MapSubwindows();
    Layout();

    SetFocusOwner(_sayentry);

//    sprintf(wname, "%s Private Message", _name);
    sprintf(wname, "%s - %s", FOXIRC_NAME, _name);
    SetWindowName(wname);

    CenterOnParent();

    MapWindow();
  }
}

OXChannel::~OXChannel() {
  _ClearHistory();
  delete[] _name;
}

//------------------------------------------------------------------------

int OXChannel::CloseWindow() {
  // Tell OXIrc to remove us from the chain...
  _server->RemoveChannel(this);
  return OXTransientFrame::CloseWindow();
}

void OXChannel::_AddView() {
  _hf = new OXCompositeFrame(this, 10, 10, HORIZONTAL_FRAME);
  _vf = new OXCompositeFrame(_hf,  10, 10, VERTICAL_FRAME);

  _hf->AddFrame(_vf, expandxexpandylayout);

  _logw = new OXViewDoc(_vf, _log = new OTextDoc(), 10, 10,
                             SUNKEN_FRAME | DOUBLE_BORDER);
  _logw->SetScrollOptions(FORCE_VSCROLL | NO_HSCROLL);
  _vf->AddFrame(_logw, expandxexpandylayout);

  AddFrame(_hf, expandxexpandylayout);

  _sayentry = new OXTextEntry(_vf, new OTextBuffer(1000), T_SAY);
  _sayentry->Associate(this);
  _sayentry->ChangeOptions(FIXED_WIDTH | SUNKEN_FRAME | DOUBLE_BORDER);
  _vf->AddFrame(_sayentry, topexpandxlayout1);
}

void OXChannel::_InitHistory() {
  _historyCurrent = 0;
  _history.clear();
}

void OXChannel::_AddToHistory(const char *str) {
  if (_history.size() > 0)
    if (strcmp(_history[_history.size() - 1], str) == 0) return;
  _history.push_back(StrDup(str));
  _historyCurrent = _history.size();
}

void OXChannel::_ClearHistory() {
  for (int i = 0; i < _history.size(); ++i) delete[] _history[i];
  _history.clear();
}

void OXChannel::Say(const char *nick, char *message, int mode) {
  char m[IRC_MSG_LENGTH];
  int  int1 = 0, int2 = 0, ctcp = false;

  m[0] = '\0';

  if (message[0] == 1) {

    for (int1 = 1; int1 < strlen(message); ++int1)
      if (message[int1] == 1) message[int1] = 0;

    if (!strncmp(message+1, "ACTION", 6)) {
      if (mode == DCC)
        sprintf(m, "%s %s", nick+1, message+8);
      else
        sprintf(m, "%s %s", nick, message+8);
      Log(m, P_COLOR_ACTION);
      return;

    } else if ((!strncmp(message+1, "PING", 4)) ||
             (!strncmp(message+1, "ECHO", 4))) {
      sscanf(message+6, "%i", &int2);
      if (mode == NOTICE) {
        sprintf(m, "PING reply from %s: %ld seconds.", nick, time(NULL)-int2);
      } else {
        sprintf(m, "NOTICE %s :\001%s\001", nick, message+1);
	_server->SendRawCommand(m);
        sprintf(m, "%s requested %s.", nick, message+1);
      }
      ctcp = true;

    } else if (!strncmp(message+1, "VERSION", 7)) {
      if (mode == NOTICE) {
        sprintf(m, "VERSION reply from %s: %s", nick, message+9);
      } else {
        sprintf(m, "VERSION %s %s %s",
                   FOXIRC_NAME, FOXIRC_VERSION, FOXIRC_HOMEPAGE);
        _server->CTCPSend(nick, m, NOTICE);
        sprintf(m, "%s requested VERSION.", nick);
      }
      ctcp = true;

    } else if (!strncmp(message+1, "CLIENTINFO", 10)) {
      if (mode == NOTICE) {
        sprintf(m, "CLIENTINFO reply from %s: %s", nick, message+12);
      } else {
        sprintf(m, "CLIENTINFO CLIENTINFO DCC ECHO VERSION");
        _server->CTCPSend(nick, m, NOTICE);
        sprintf(m, "%s requested CLIENTINFO.", nick);
      }
      ctcp = true;

    } else if (!strncmp(message+1, "DCC", 3)) {
      if (mode == NOTICE) {
        sprintf(m, "DCC reply from %s: %s", nick, message+1);
      } else {
        sprintf(m, "%s requested DCC: %s.", nick, message+1);
        ProcessDCCRequest(nick, message+1);
      }

    } else {
//      _server->CTCPSend(nick, message+1, PRIVMSG);
      sprintf(m, "%s CTCP: %s", nick, message+1);
      ctcp = true;
    }

    if (ctcp && foxircSettings->SendToInfo(P_LOG_CTCP))
      _server->Log(m, P_COLOR_CTCP);
    else
      Log(m, P_COLOR_CTCP);

//  } else if (!strcasecmp(nick, _name)) {
//    sprintf(m, "\003%d=\003%d%s\003%d=\003%d %s", 2, 11, nick, 2, 0, message);
//    //sprintf(m, "%s", message);

  } else if (!strcasecmp(nick, _name)) { // for MSG windows
    if (mode == DCC)
      sprintf(m, "\003%d=\003%d%s\003%d=\003 %s", 9, 15, nick+1, 9, message);
//    else if (mode == NOTICE)
//      sprintf(m, "\003%d-\003%d%s\003%d-\003 %s", 9, 15, nick, 9, message);
    else
      sprintf(m, "\003%d<\003%d%s\003%d>\003 %s", 9, 15, nick, 9, message);
    Log(m);    

  } else if (!strcasecmp(nick, _server->GetNick())) {  // our own text
    if (mode == DCC)
      sprintf(m, "\003%d<\003%d%s\003%d>\003 %s", 8, 15, nick, 8, message);
    else if (mode == NOTICE)
      sprintf(m, "\003%d-\003%d%s\003%d-\003 %s", 8, 15, nick, 8, message);
    else
      sprintf(m, "\003%d<\003%d%s\003%d>\003 %s", 8, 15, nick, 8, message);
    Log(m);

  } else {  // other's text
    
    if (mode == NOTICE) {
      if (*nick) {
        sprintf(m, "\003%d-\003%d%s\003%d-\003 %s", 11, 15, nick, 11, message);
      } else {
        sprintf(m, "%s", message);
        Log(m, P_COLOR_NOTICE);
        return;
      }
    } else {
      sprintf(m, "\003%d<\003%d%s\003%d>\003 %s", 11, 15, nick, 11, message);
    }
      
    //sprintf(m, "<%s> %s", nick, message);
    Log(m);

  }

  if (strchr(m, '\007')) XBell(GetDisplay(), 0);
}

void OXChannel::Log(const char *message) {
  Log(message, P_COLOR_TEXT);
}

void OXChannel::Log(const char *message, int color) {

//  if (foxircSettings->CheckMisc(P_MISC_POPUP_WINDOW)) RaiseWindow();
//  if (foxircSettings->CheckChannelFlags(_name, AUTO_RAISE_WINDOW)) RaiseWindow();
  if (_cinfo->flags & AUTO_RAISE_WINDOW) RaiseWindow();

  OLineDoc *l = new OLineDoc();
  l->SetCanvas(_log->GetTextFrame());
  l->SetDefaultColor(foxircSettings->GetPixelID(color));
  l->Fill(message);
  _log->AddLine(l);
  _logw->ScrollUp();
}

void OXChannel::StartDCCChat(const char *nick) {
  char char1[20], char2[256];
  unsigned long hostnum;
  unsigned short portnum;

  sprintf(char1, "=%s", nick);
  OXDCCChannel *xdcc = (OXDCCChannel *) _server->GetChannel(char1);

  if (!xdcc->Listen(&hostnum, &portnum)) {
    xdcc->Log("Failed to create server socket", P_COLOR_NOTIFY);
    printf("StartDCCChat: Listen Failed!\n");
  } else {
    printf("Listening for DCC Chat with %s on IP %u Port %d\n",
           nick, ntohl(hostnum), ntohs(portnum));
    sprintf(char2, "DCC CHAT chat %u %d", ntohl(hostnum), ntohs(portnum));
    _server->CTCPSend(nick, char2, PRIVMSG);  
  }
}

void OXChannel::AcceptDCCChat(const char *nick,
                              const char *server, int port) {
  char char1[IRC_MSG_LENGTH];

  sprintf(char1, "=%s", nick);
  OXDCCChannel *xdcc = (OXDCCChannel *) _server->GetChannel(char1);
  xdcc->Connect(server, port);
}

void OXChannel::ProcessDCCRequest(const char *nick, const char *string) {
  int  ret;
  char char1[IRC_MSG_LENGTH], char2[IRC_MSG_LENGTH],
       char3[IRC_MSG_LENGTH], char4[IRC_MSG_LENGTH],
       char5[IRC_MSG_LENGTH], char6[IRC_MSG_LENGTH];

  if (sscanf(string, "DCC %s %s %s %s %s",
                     char1, char2, char3, char4, char5) == 5) {
    printf("ahh a DCC send\n");
    new OXDCCFile(_client->GetRoot(), nick, char2, char3, char4, char5, &ret);

//    sprintf(char6, "%s wishes to send you %s which is %s bytes in size", 
//            nick, char2, char5);
//    new OXMsgBox(_client->GetRoot(), this, new OString("DCC Confirm"), 
//                 new OString(char6), MB_ICONQUESTION,
//                 ID_YES | ID_NO | ID_IGNORE, &ret);
//    if (ret == ID_YES) {
//      printf("Accepting %s from %s\n", char2, nick);
      //AcceptDCCFile(nick, char2, char3, char4, char5);

//    } else if (ret == ID_NO) {
//      printf("Refusing %s from %s\n", char2, nick);
//
//    } else {
//      printf("Ignoring %s from %s\n", char2, nick);
//    }

    if (ret == ID_DCC_REJECT) {
      printf("Refusing send from %s\n", nick);
      sprintf(char1, "DCC REJECT SEND %s", char2);
      _server->CTCPSend(nick, char1, NOTICE);

    }

  } else if (sscanf(string, "DCC %s %s %s %s",
                             char1, char2, char3, char4) == 4) {
    printf("ahh a DCC chat\n");
    sprintf(char6, "%s wishes to chat with you ", nick);
    new OXMsgBox(_client->GetRoot(), this, new OString("DCC Confirm"), 
                 new OString(char6), MB_ICONQUESTION,
                 ID_YES | ID_NO | ID_IGNORE, &ret);
    if (ret == ID_YES) {
      printf("Accepting chat from %s\n", nick);
      AcceptDCCChat(nick, char3, atoi(char4));

    } else if (ret == ID_NO) {
      printf("Refusing chat from %s\n", nick);
      _server->CTCPSend(nick, "DCC REJECT CHAT chat", NOTICE);

    } else {
      printf("Ignoring chat from %s\n", nick);

    }

  } else if (sscanf(string, "DCC REJECT %s %s", char1, char2) == 2) {
    if (strcasecmp(char1, "chat") == 0) { 
      _server->CTCPSend(nick, "DCC REJECT CHAT chat", NOTICE);
      printf("A chat reject from %s\n", nick);
      sprintf(char3, "=%s", nick);
      OXChannel *ch = _server->FindChannel(char3);
      if (ch) ((OXDCCChannel *) ch)->Disconnect();
    }
                                
  }
}

int OXChannel::ProcessMessage(OMessage *msg) {
  char char1[IRC_MSG_LENGTH], char3[IRC_MSG_LENGTH];
  char *strtmp;

  switch (msg->type) {
    case MSG_TEXTENTRY:
      {
      OTextEntryMessage *tmsg = (OTextEntryMessage *) msg;
      switch (msg->action) {
        case MSG_TEXTCHANGED:

          switch (tmsg->keysym) {
            case XK_Return:

              strcpy(char1, _sayentry->GetString());
              if (strlen(char1) > 0) {
                if (!ProcessCommand(char1)) {
                  strtmp = strtok(char1, "\n");
		  while (strtmp) {
                    sprintf(char3, "PRIVMSG %s :%s", _name, strtmp);
                    _server->SendRawCommand(char3);
                    Say(_server->GetNick(), strtmp, PRIVMSG);
                    _AddToHistory(strtmp);
                    strtmp = strtok(NULL, "\n");
                  }
		} else if (*char1 == '/') {
                  _AddToHistory(char1);
                }
		_historyCurrent = _history.size();
	        _sayentry->Clear();
              }
	      break;

	    case XK_Up:
              if ((_history.size() > 0) &&
                  (_historyCurrent > 0)) {
                _sayentry->Clear();
                _sayentry->AddText(0, _history[--_historyCurrent]);
              }
              break;

	    case XK_Down:
              _sayentry->Clear();
              if ((_history.size() > 0) &&
                  (_historyCurrent < _history.size() - 1)) {
                _sayentry->AddText(0, _history[++_historyCurrent]);
              } else {
                _historyCurrent = _history.size();
              }
	      break;
          }
          break;
      }
      }
      break;

    case INCOMING_IRC_MSG:
      {
        OIrcMessage *ircmsg = (OIrcMessage *) msg;

        switch (msg->action) {
          default:
	    break;
        }
      }
      break;
  }
  return True;
}

int OXChannel::ProcessCommand(char *cmd) {
  char char1[IRC_MSG_LENGTH],
       char2[IRC_MSG_LENGTH];

  if (!cmd || *cmd != '/') return False;

  if (!strncmp(cmd, "/me ", 4)) {
    sprintf(char1, "\1ACTION %s\1", &cmd[4]);
    sprintf(char2, "PRIVMSG %s :%s", _name, char1);
    _server->SendRawCommand(char2);
    Say(_server->GetNick(), char1, PRIVMSG);

  } else if (!strncmp(cmd, "/notice ", 8)) {
    sprintf(char1, "NOTICE %s :%s", _name, &cmd[8]);
    _server->SendRawCommand(char1);
    Say(_server->GetNick(), &cmd[8], NOTICE);

  } else if (!strncmp(cmd, "/msg ", 5)) {
    if (strlen(&cmd[5]) > 0)
      _server->GetChannel(&cmd[5])->MapRaised();

  } else if (!strncmp(cmd, "/raw ", 5)) {
    if (strlen(&cmd[5]) > 0)
      _server->SendRawCommand(&cmd[5]);

  } else if (!strncmp(cmd, "/quote ", 7)) {
    if (strlen(&cmd[7]) > 0)
      _server->SendRawCommand(&cmd[7]);

  } else if (!strncmp(cmd, "/whois ", 7)) {
    if (strlen(&cmd[7]) > 0)
      _server->SendRawCommand(&cmd[1]);

  } else if (!strncmp(cmd, "/whowas ", 8)) {
    if (strlen(&cmd[8]) > 0)
      _server->SendRawCommand(&cmd[1]);

  }

  return True;
}
