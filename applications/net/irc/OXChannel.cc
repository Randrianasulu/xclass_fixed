#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <errno.h>

#include "OXDCCFile.h"
#include "IRCcodes.h"
#include "OIrcMessage.h"
#include "OXChannel.h"
#include "OXIrc.h"
#include "OXConfirmDlg.h"

#include "OXDCCChannel.h" //////


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

OXChannel::OXChannel(const OXWindow *p, const OXWindow *main, OXIrc *s,
                     const char *ch, int init) :
  OXTransientFrame(p, main, 1, 1) {
  char wname[100];
  int  ax, ay;
  Window wdummy;

  _server = s;
  _next = _prev = NULL;
  _name = StrDup(ch);
  
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
    sprintf(wname, "fOXIrc - %s", _name);
    SetWindowName(wname);

    //---- position relative to the parent's window
 
    XTranslateCoordinates(GetDisplay(),
                          s->GetId(), GetParent()->GetId(),
                          50, 50, &ax, &ay, &wdummy);

    Move(ax, ay);

    MapWindow();
  }
}

OXChannel::~OXChannel() {
  delete[] _name;
}

//------------------------------------------------------------------------

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

void OXChannel::CloseWindow() {
  // Tell OXIrc to remove us from the chain...
  _server->RemoveChannel(this);
  OXTransientFrame::CloseWindow();
}

void OXChannel::Say(char *nick, char *message, int mode) {
  char m[TCP_BUFFER_LENGTH];
  int  int1 = 0, int2 = 0;

  m[0] = '\0';
  if (message[0] == 1) {
    for (int1=1; int1<strlen(message); int1++)
      if (message[int1] == 1) message[int1] = 0;

    if (!strncmp(message+1, "ACTION", 6)) {
      if (mode == DCC)
        sprintf(m, "%s %s", nick+1, message+8);
      else
        sprintf(m, "%s %s", nick, message+8);
      Log(m, 1);
      return;
    } else if ((!strncmp(message+1, "PING", 4)) ||
             (!strncmp(message+1, "ECHO", 4))) {
      sscanf(message+6, "%i", &int2);
      if (mode == NOTICE) {
        sprintf(m, "PING reply from %s: %i seconds.", nick, time(NULL)-int2);
      } else {
        sprintf(m, "NOTICE %s :\001%s\001\n", nick, message+1);
	OTcpMessage msg(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, m);
	SendMessage(_server, &msg);
        sprintf(m, "%s requested %s.", nick, message+1);
      }
    } else if (!strncmp(message+1, "VERSION", 7)) {
      if (mode == NOTICE) {
        sprintf(m, "VERSION reply from %s: %s", nick, message+9);
      } else {
        sprintf(m, "VERSION %s", FOXIRCVERSION);
        _server->CTCPSend(nick, m, NOTICE);
        sprintf(m, "%s requested VERSION.", nick);
      } 
    } else if (!strncmp(message+1, "CLIENTINFO", 10)) {
      if (mode == NOTICE) {
        sprintf(m, "CLIENTINFO reply from %s: %s", nick, message+12);
      } else {
        sprintf(m, "CLIENTINFO CLIENTINFO DCC ECHO VERSION");
        _server->CTCPSend(nick, m, NOTICE);
        sprintf(m, "%s requested CLIENTINFO.", nick);
      }
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
    }
    Log(m, 2);
//  } else if (!strcasecmp(nick, _name)) {
//    sprintf(m, "%c2=%c11%s%c2=%c0 %s", 3, 3, nick, 3, 3, message);
//    //sprintf(m, "%s", message);
  } else if (!strcasecmp(nick, _name)) { // for MSG windows
    if (mode == DCC)
      sprintf(m, "%c9=%c15%s%c9=%c %s", 3, 3, ((char *)nick)+1, 3, 3, message);
    else
      sprintf(m, "%c13<%c15%s%c13>%c %s", 3, 3, nick, 3, 3, message);
    Log(m);    
  } else if (!strcasecmp(nick, _server->_nick)) {
    if (mode == DCC)
      sprintf(m, "%c9<%c15%s%c9>%c %s", 3, 3, nick, 3, 3, message);
    else
      sprintf(m, "%c13<%c15%s%c13>%c %s", 3, 3, nick, 3, 3, message);
    Log(m);
  } else {
    sprintf(m, "%c12<%c15%s%c12>%c %s", 3, 3, nick, 3, 3, message);
    //sprintf(m, "<%s> %s", nick, message);
    Log(m);
  }
  if (strchr(m, '\007')) XBell(GetDisplay(), 0);
}

void OXChannel::Log(char *message) {
  Log(message, 11);
}

void OXChannel::Log(char *message, char *color) {

//  if (foxircSettings->CheckMisc(P_MISC_POPUP_WINDOW)) RaiseWindow();
// if (foxircSettings->CheckChannelFlags(_name,AUTO_RAISE_WINDOW)) RaiseWindow();
  if (_cinfo->flags & AUTO_RAISE_WINDOW ) RaiseWindow();

  OLineDoc *l = new OLineDoc();
  l->SetCanvas(_log->_style_server);
  l->SetColor(color);
  l->Fill(message);
  _log->AddLine(l);
  _logw->ScrollUp();
}
void OXChannel::Log(char *message, int color) {
  if (_cinfo->flags & AUTO_RAISE_WINDOW ) RaiseWindow();

  OLineDoc *l = new OLineDoc();
  l->SetCanvas(_log->_style_server);
  l->SetColor(foxircSettings->GetColorID(color));
  l->Fill(message);
  _log->AddLine(l);
  _logw->ScrollUp();
}

void OXChannel::StartDCCChat(char *nick) {
  char char1[20], char2[256];
  unsigned long hostnum;
  unsigned short portnum;

  sprintf(char1, "=%s", nick);
  OXDCCChannel *xdcc = (OXDCCChannel *) _server->GetChannel(char1);

  if (!xdcc->Listen(&hostnum, &portnum)) {
    _server->GetChannel(char1)->Log("Failed to create server socket", "yellow");
    printf("StartDCCChat: Listen Failed!\n");
  } else {
    printf("Listening for DCC Chat with %s on IP %lu Port %d\n",
           nick, ntohl(hostnum), ntohs(portnum));
    sprintf(char2, "DCC CHAT chat %lu %d", ntohl(hostnum), ntohs(portnum));
    _server->CTCPSend(nick, char2, PRIVMSG);  
  }
}

void OXChannel::AcceptDCCChat(char *inick, char *iserver, int iport) {
  char char1[TCP_BUFFER_LENGTH];

  sprintf(char1, "=%s", inick);
  OXDCCChannel *xdcc = (OXDCCChannel *) _server->GetChannel(char1);
  xdcc->Connect(iserver, iport);
}

void OXChannel::ProcessDCCRequest(char *nick, char *string) {
  int  ret;
  char char1[TCP_BUFFER_LENGTH], char2[TCP_BUFFER_LENGTH],
       char3[TCP_BUFFER_LENGTH], char4[TCP_BUFFER_LENGTH],
       char5[TCP_BUFFER_LENGTH], char6[TCP_BUFFER_LENGTH];

  if (sscanf(string, "DCC %s %s %s %s %s",
                     char1, char2, char3, char4, char5) == 5) {
    printf("ahh a DCC send\n");
    OXDCCFile *tp = new OXDCCFile(_client->GetRoot(), nick, char2, char3, char4, char5);

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

  } else if (sscanf(string, "DCC %s %s %s %s",
                             char1, char2, char3, char4, char5) == 4) {
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
      printf("A Rejecting chat from %s\n", nick);
      sprintf(char3, "=%s", nick);
      OXChannel *ch = _server->FindChannel(char3);
      if (ch) ((OXDCCChannel *) ch)->CloseUp();
    }
                                
  }
}

int OXChannel::ProcessMessage(OMessage *msg) {
  char char1[TCP_BUFFER_LENGTH], 
       char3[TCP_BUFFER_LENGTH],
       char4[TCP_BUFFER_LENGTH],
       m[TCP_BUFFER_LENGTH];
  char *strtmp;
  int n;

  switch(msg->type) {
    case MSG_TEXTENTRY:
      {
      OTextEntryMessage *tmsg = (OTextEntryMessage *) msg;
      switch(msg->action) {
        case MSG_TEXTCHANGED:

          switch(tmsg->keysym) {
            case XK_Return:
              strcpy(char1, _sayentry->GetString());
              if (strlen(char1) > 0) {
		strtmp = strtok(char1, "\n");
		while (strtmp) {
                  sprintf(char3, "PRIVMSG %s :%s\n", _name, strtmp);
                  OIrcMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char3);
                  SendMessage(_server, &message);
                  Say(_server->_nick, strtmp, PRIVMSG);
                  _history.Add(StrDup(strtmp));
                  strtmp = strtok(NULL, "\n");
		}
		historyCurrent = _history.GetSize();
	        _sayentry->Clear();
              }
	      break;

	    case XK_Up:
	      _sayentry->Clear();
	      if ((_history.GetAt(historyCurrent)) && (historyCurrent > 0))
                _sayentry->AddText(0, _history.GetAt(historyCurrent--));
              break;

	    case XK_Down:
	      _sayentry->Clear();
	      if ((_history.GetAt(historyCurrent)) && 
                  (_history.GetSize() > historyCurrent)) 
                _sayentry->AddText(0, _history.GetAt(historyCurrent++));
	      break;
          }
          break;
      }
      }
      break;

    case INCOMING_IRC_MSG:
      {
        OIrcMessage *ircmsg = (OIrcMessage*)msg;

        switch(msg->action) {
          default:
	    break;
        }
      }
      break;
  }
  return True;
}

/*
void OXChannel::HandleModeChange(char *mode) {
  char char1[TCP_BUFFER_LENGTH],
       char2[TCP_BUFFER_LENGTH],
       char3[TCP_BUFFER_LENGTH],
       char4[TCP_BUFFER_LENGTH],
       char5[TCP_BUFFER_LENGTH],
       char6[TCP_BUFFER_LENGTH];

  if (sscanf(mode, "+%s %s ", char1, char2) == 2) {

  } else
    if(sscanf(mode,"-%s %s ", char1, char2) == 2) {
}
*/
