#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <errno.h>

#include "IRCcodes.h"
#include "OIrcMessage.h"
#include "OXChatChannel.h"
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

extern OXPopupMenu *_nick_actions, *_nick_ctcp;

extern OSettings  *foxircSettings;

extern char *filetypes[];

//----------------------------------------------------------------------

OXChatChannel::OXChatChannel(const OXWindow *p, const OXWindow *main, 
                             OXIrc *s, const char *ch) :
  OXChannel(p, main, s, ch, False) {
  char wname[100];
  int  ax, ay;
  Window wdummy;

  _logfile = NULL;
  _logfilename = NULL;
   historyCurrent = 0;
  _cmode = _ecmode = _umode = _eumode = 0L;

  _menulog = new OXPopupMenu(_client->GetRoot());
  _menulog->AddEntry(new OHotString("&Open..."),  L_OPEN);
  _menulog->AddEntry(new OHotString("&Close"), L_CLOSE);
  _menulog->AddEntry(new OHotString("&Flush"), L_FLUSH);
  _menulog->AddSeparator();
  _menulog->AddEntry(new OHotString("&Empty"), L_EMPTY);
  _menulog->DisableEntry(L_CLOSE);
  _menulog->DisableEntry(L_FLUSH);
  _menulog->DisableEntry(L_EMPTY);

  _menumode = new OXPopupMenu(_client->GetRoot());
  _menumode->AddEntry(new OHotString("Pop &Up"),      M_POPUP);
  _menumode->AddEntry(new OHotString("Pop &Down"),    M_POPDOWN);
  _menumode->AddEntry(new OHotString("Dra&w"),        M_DRAW);
  _menumode->AddEntry(new OHotString("&Quiet"),       M_QUIET);
  _menumode->AddEntry(new OHotString("&Actions"),     M_ACTIONS);
  _menumode->AddEntry(new OHotString("&Ban"),         M_BAN);
  _menumode->AddEntry(new OHotString("&Private"),     M_PRIVATE);
  _menumode->AddEntry(new OHotString("&Moderated"),   M_MODERATED);
  _menumode->AddEntry(new OHotString("&Secret"),      M_SECRET);
  _menumode->AddEntry(new OHotString("&Invite Only"), M_INVITEONLY);
  _menumode->AddEntry(new OHotString("&Topic"),       M_TOPIC);
  _menumode->AddEntry(new OHotString("&No Msg"),      M_NOMSG);
  _menumode->AddEntry(new OHotString("&Limit"),       M_LIMIT);
  _menumode->AddEntry(new OHotString("&Key"),         M_KEY);
  _menumode->DisableEntry(M_POPUP);
  _menumode->DisableEntry(M_POPDOWN);
  _menumode->DisableEntry(M_DRAW);
  _menumode->DisableEntry(M_QUIET);
  _menumode->DisableEntry(M_ACTIONS);
  _menumode->DisableEntry(M_BAN);
  _menumode->DisableEntry(M_PRIVATE);
  _menumode->DisableEntry(M_MODERATED);
  _menumode->DisableEntry(M_SECRET);
  _menumode->DisableEntry(M_INVITEONLY);
  _menumode->DisableEntry(M_TOPIC);
  _menumode->DisableEntry(M_NOMSG);
  _menumode->DisableEntry(M_LIMIT);
  _menumode->DisableEntry(M_KEY);

  _menuchannel = new OXPopupMenu(_client->GetRoot());
  _menuchannel->AddPopup(new OHotString("&Mode"),    _menumode);
  _menuchannel->AddSeparator();
  _menuchannel->AddEntry(new OHotString("&Who"),     CH_WHO);
  _menuchannel->AddEntry(new OHotString("&Invite"),  CH_INVITE);
  _menuchannel->AddEntry(new OHotString("&Notice"),  CH_NOTICE);
  _menuchannel->AddEntry(new OHotString("&Draw"),    CH_DRAW);
  _menuchannel->AddPopup(new OHotString("&CTCP"),    _nick_ctcp);
  _menuchannel->AddPopup(new OHotString("L&og"),     _menulog);
  _menuchannel->AddEntry(new OHotString("&History"), CH_HISTORY);
  _menuchannel->AddEntry(new OHotString("&Exec"),    CH_EXEC);
  _menuchannel->AddSeparator();
  _menuchannel->AddEntry(new OHotString("&Leave"),   CH_LEAVE);

  _menuedit = new OXPopupMenu(_client->GetRoot());
  _menuedit->AddEntry(new OHotString("Cu&t"), 1);
  _menuedit->AddEntry(new OHotString("&Copy"), 1);
  _menuedit->AddSeparator();
  _menuedit->AddEntry(new OHotString("Select &All"), 1);
  _menuedit->AddEntry(new OHotString("&Invert Selection"), 1);
  
  _menuview = new OXPopupMenu(_client->GetRoot());
  _menuview->AddEntry(new OHotString("&Toolbar"),    801);
  _menuview->AddEntry(new OHotString("Status &Bar"), 802);
  _menuview->AddEntry(new OHotString("T&opic"),      803);
  _menuview->AddSeparator();
  _menuview->AddEntry(new OHotString("&Fonts..."),   1);
  _menuview->AddEntry(new OHotString("&Colors..."),  1);

  _menuview->CheckEntry(802);
  _menuview->CheckEntry(803);

  _menuhelp = new OXPopupMenu(_client->GetRoot());   
  _menuhelp->AddEntry(new OHotString("&Contents..."), 1);
  _menuhelp->AddEntry(new OHotString("&Search..."), 1);
  _menuhelp->AddSeparator();
  _menuhelp->AddEntry(new OHotString("&About"), 1);  

  _menulog->Associate(this);
  _menuchannel->Associate(this);
  _menumode->Associate(this);
  _menuedit->Associate(this);
  _menuview->Associate(this);
  _menuhelp->Associate(this);

  _menubar = new OXMenuBar(this, 1, 1, HORIZONTAL_FRAME);

  _menubar->AddPopup(new OHotString("&Channel"), _menuchannel, menuitemlayout);
  _menubar->AddPopup(new OHotString("&Edit"), _menuedit, menuitemlayout);
  _menubar->AddPopup(new OHotString("&View"), _menuview, menuitemlayout);
  _menubar->AddPopup(new OHotString("&Help"), _menuhelp, menuitemlayout);

  AddFrame(_menubar, menubarlayout);

  //---- topic entry

  _topicsep = new OXHorizontal3dLine(this);
  AddFrame(_topicsep, topexpandxlayout3);

  _ftopic = new OXCompositeFrame(this, 10, 10, HORIZONTAL_FRAME);

  _ltopic = new OXLabel(_ftopic, new OString("Topic: "));
  _ftopic->AddFrame(_ltopic, leftcenterylayout);

  _topic = new OXTextEntry(_ftopic, new OTextBuffer(1000), T_TOPIC);
  _topic->Associate(this);
  //_topic->ChangeOptions(FIXED_WIDTH | SUNKEN_FRAME | DOUBLE_BORDER);
  _ftopic->AddFrame(_topic, expandxexpandylayout);

  AddFrame(_ftopic, topexpandxlayout2);

  //---- view and say entry

  _AddView();

  //---- name list

  _canvas = new OXCanvas(_hf, 110, 1, FIXED_WIDTH);
  _canvas->GetViewPort()->SetBackgroundPixmap(ParentRelative);
  _nlist = new OXNameList(_canvas->GetViewPort(), _nick_actions, 1, 110,
                                                  FIXED_WIDTH);
  _nlist->Associate(this);
  _canvas->SetContainer(_nlist);
  _hf->AddFrame(_canvas, leftexpandylayout);

  //---- status bar

  _statusBar = new OXStatusBar(this);
  AddFrame(_statusBar, new OLayoutHints(LHINTS_BOTTOM | LHINTS_EXPAND_X,
                               0, 0, 3, 0));

  _statusBar->SetWidth(0, 250);
  _statusBar->AddLabel(200);
  _statusBar->AddLabel(100, LHINTS_RIGHT);

  _statusBar->SetText(0, new OString("Channel Modes"));
  _statusBar->SetText(1, new OString("Not an Op"));
  _statusBar->SetText(2, new OString("O:? V:? N:? I:? B:?"));

//  sprintf(wname, "IRC channel %s", _name);
  sprintf(wname, "%s IRC channel", _name);
  SetWindowName(wname);
  //_focusMgr->SetFocusOwner(_sayentry);
  SetFocusOwner(_sayentry);

  Resize(610, 362);

  MapSubwindows();
  Layout();

  //---- position relative to the parent's window

  XTranslateCoordinates(GetDisplay(),
                        s->GetId(), GetParent()->GetId(),
                        60, 60, &ax, &ay, &wdummy);

  Move(ax, ay);

  MapWindow();
}

//----------------------------------------------------------------------

OXChatChannel::~OXChatChannel() {
  if (_logfile) DoCloseLog();
}

//----------------------------------------------------------------------

int OXChatChannel::CloseWindow() {
  return DoLeave();
}

//----------------------------------------------------------------------

int OXChatChannel::ProcessMessage(OMessage *msg) {
  char char1[TCP_BUFFER_LENGTH],
       char2[TCP_BUFFER_LENGTH],
       char3[TCP_BUFFER_LENGTH],
       char4[TCP_BUFFER_LENGTH],
       m[TCP_BUFFER_LENGTH];
  char *strtmp;
  int n;
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;
  OTextEntryMessage *tmsg = (OTextEntryMessage *) msg;

  switch(msg->type) {
    case MSG_MENU:
      switch(msg->action) {
        case MSG_CLICK:
          switch(wmsg->id) {
            case CH_LEAVE:
              DoLeave();
              break;

            case L_OPEN:  DoOpenLog();  break;
            case L_CLOSE: DoCloseLog(); break;
            case L_FLUSH: DoFlushLog(); break;
            case L_EMPTY: DoEmptyLog(); break;
	    case CTCP_PING:
                sprintf(char1, "PING %ld", time(NULL));
                _server->CTCPSend((char *)_name,
		                  char1, PRIVMSG);
                break;
	    case CTCP_VERSION:
	         _server->CTCPSend((char *)_name,"VERSION", PRIVMSG);
                break;
	    case CTCP_CLIENTINFO:
	         _server->CTCPSend((char *)_name,"CLIENTINFO", PRIVMSG);
                break;

            case 802:
              DoToggleStatusBar();
              break;

            case 803:
              DoToggleTopicBar();
              break;

            default:
              break;
          }
          break;

        default:
          break;
      }
      break;

    case MSG_TEXTENTRY:
      if (msg->action == MSG_TEXTCHANGED) {
        if (tmsg->id == T_SAY) {
          if (tmsg->keysym == XK_Return) {
            strcpy(char1, _sayentry->GetString());
            if (strlen(char1) > 0) {
              if (!ProcessCommand(char1)) {
//                strcpy(char2, char1);
		strtmp=strtok(char1,"\n");
		while(strtmp){
                sprintf(char3, "PRIVMSG %s :%s\n", _name, strtmp);
                OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char3);
                SendMessage(_server, &message);
                Say(_server->_nick, strtmp, PRIVMSG);
		_history.Add(StrDup(strtmp));
		strtmp=strtok(NULL,"\n");
		}
              }
            }
            historyCurrent = _history.GetSize();
            _sayentry->Clear();
          } else if (tmsg->keysym == XK_Up) {
	  // get the last entry in the history;
		_sayentry->Clear();
		if( ( _history.GetAt(historyCurrent) ) &&
		    ( historyCurrent>0) )
			_sayentry->AddText(0,_history.GetAt(historyCurrent--));

	  } else if (tmsg->keysym == XK_Down) {
		_sayentry->Clear();
		if( ( _history.GetAt(historyCurrent) ) &&
		    ( _history.GetSize()>historyCurrent) ) 
		   		_sayentry->AddText(0,_history.GetAt(historyCurrent++));
        	}
	} else if (tmsg->id == T_TOPIC) {
          if (tmsg->keysym == XK_Return) {
            strcpy(char1, _topic->GetString());
            OIrcMessage message(OUTGOING_IRC_MSG, TOPIC_SET,
                                0, 0, 0, 0, _name, char1);
            SendMessage(_server, &message);
            _topic->Clear();
          }
        }
      }
      break;

    case IRC_NICKLIST:
      {
        OIrcMessage *ircmsg = (OIrcMessage *) msg;

        switch (msg->action) {
          case MSG_CLICK:
            switch(ircmsg->parm1) {
              case C_PING:
                sprintf(char1, "PING %ld", time(NULL));
                _server->CTCPSend((char *)(ircmsg->string1->GetString()),
		                  char1, PRIVMSG);
                break;

              case C_WHOIS:
                sprintf(char1, "WHOIS %s\n",
		               ircmsg->string1->GetString());
	        {
	          OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
	          SendMessage(_server, &message);
	        }
                break;

              case C_MESSAGE:
                _server->GetChannel(ircmsg->string1->GetString())->MapRaised();
                break;

              case CTCP_VERSION:
                _server->CTCPSend((char *)(ircmsg->string1->GetString()),
                                  "VERSION", PRIVMSG);
                break;

              case CTCP_CLIENTINFO:
                _server->CTCPSend((char *)(ircmsg->string1->GetString()),
                                  "CLIENTINFO", PRIVMSG);
                break;

              case DCC_CHAT:
                StartDCCChat((char *)(ircmsg->string1->GetString()));
                break;

              default:
                printf("command %i %s\n", 
		       ircmsg->parm1, ircmsg->string1->GetString());
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
          case JOIN:
            _nlist->AddName(ircmsg->string2->GetString());
            if (ircmsg->string3 &&
                strlen(ircmsg->string2->GetString()) > 0)
              sprintf(m, "%s (%s) joined the channel.",
                         ircmsg->string2->GetString(),
                         ircmsg->string3->GetString());
            else
              sprintf(m, "%s joined the channel.",
                         ircmsg->string2->GetString());
            Log(m, 7);
            _canvas->Layout();
	    break;

          case LEAVE:
            _nlist->RemoveName(ircmsg->string2->GetString());
            if (ircmsg->string3->GetLength() > 0)
              sprintf(m, "%s left the channel (%s).",
                         ircmsg->string2->GetString(),
                         ircmsg->string3->GetString());
            else
              sprintf(m, "%s left the channel.",
                         ircmsg->string2->GetString());
            Log(m, 16);
            _canvas->Layout();
            if (strcmp(ircmsg->string2->GetString(), _server->_nick) == 0)
              OXChannel::CloseWindow();
	    break;

          case KICK:
            _nlist->RemoveName(ircmsg->string3->GetString());
            if (strcmp(ircmsg->string3->GetString(), _server->_nick) == 0)
              sprintf(m, "You have been kicked off the channel by %s (%s).", 
                         ircmsg->string2->GetString(), 
                         ircmsg->string4->GetString());
            else
              sprintf(m, "%s has been kicked off the channel by %s (%s).", 
                         ircmsg->string3->GetString(),
                         ircmsg->string2->GetString(), 
                         ircmsg->string4->GetString());
            Log(m, 8);
            _canvas->Layout();
	    break;

          case QUIT:
	    {
	      if(_nlist->HaveNick(ircmsg->string1->GetString())){
	            _nlist->RemoveName(ircmsg->string1->GetString());
            	if (ircmsg->string2->GetLength() > 0)
            	  sprintf(m, "SignOff: %s (%s).",
            	             ircmsg->string1->GetString(),
            	             ircmsg->string2->GetString());
            	else
            	  sprintf(m, "SignOff: %s",
            	             ircmsg->string1->GetString());
            	Log(m, 17);
            	_canvas->Layout();
	      }
	    }
	    break;

          case NICK:
            if ((_nlist) &&
                (_nlist->GetName(ircmsg->string2->GetString()))) {
              _nlist->ChangeName(ircmsg->string2->GetString(),
                                 ircmsg->string1->GetString());
              sprintf(m, "%s is now known as %s.", 
                         ircmsg->string2->GetString(), 
                         ircmsg->string1->GetString());
              Log(m, 10);
	    }
	    break;

          case NICKLIST:
	    {
              char *s;
              int i,j;

              if (_nlist) {
                for(i=0, s=(char*)(ircmsg->string2->GetString());
		    sscanf(s, "%s %n", &m, &j) == 1;
		    s += j) _nlist->AddName(m);
                _canvas->Layout();
              }
	    }
	    break;

          case MODE:
            strcpy(char1, ircmsg->string3->GetString());
            if (char1[strlen(char1)-1] == ' ') char1[strlen(char1)-1] = '\0';
            sprintf(m, "Mode change \"%s\" by %s",
                       char1, ircmsg->string2->GetString());
            Log(m, 9);
	    DoChannelMode(char1);
	    //HandleModeChange(char1);
            break;

          case TOPIC_SET:
            _topic->Clear();
            _topic->AddText(0, ircmsg->string3->GetString());
            sprintf(m, "%s has set the topic: \"%s\"",
                       ircmsg->string2->GetString(),
                       ircmsg->string3->GetString());
            Log(m, 18);
            break;

          case TOPIC_SETBY:
            strcpy(char1, ctime((time_t *) &ircmsg->parm1));
            if (char1[strlen(char1)-1] == '\n') char1[strlen(char1)-1] = '\0';
            sprintf(m, "The topic was set by %s on %s",
                       ircmsg->string1->GetString(), char1);
            Log(m, 18);
            break;

          case TOPIC:
            _topic->Clear();
            _topic->AddText(0, ircmsg->string1->GetString());
            break;

          case SPECIAL:
            switch (ircmsg->parm1) {
              case 329:  // channel creation time
                strcpy(char1, ctime((time_t *) &ircmsg->parm2));
                if (char1[strlen(char1)-1] == '\n') 
                  char1[strlen(char1)-1] = '\0';
                sprintf(m, "The channel was created on %s", char1);
                Log(m, 9);
                break;

              case ERR_CHANOPRIVSNEEDED:
                Log("You are not a channel operator.", 19);
                break;

              default:
                break;
            }

        }
      }
      break;
  }
  return True;
}

//----------------------------------------------------------------------

int OXChatChannel::ProcessCommand(char *cmd) {
  char char1[TCP_BUFFER_LENGTH],
       char2[TCP_BUFFER_LENGTH];

  if (!cmd || *cmd != '/') return False;

  if (!strncmp(cmd, "/me ", 4)) {
    sprintf(char1, "\1ACTION %s\1", &cmd[4]);
    sprintf(char2, "PRIVMSG %s :%s\n", _name, char1);
    {
      OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char2);
      SendMessage(_server, &message);
    }
    Say(_server->_nick, char1, PRIVMSG);
  } else if (!strncmp(cmd, "/names", 4)) {
    {
      OIrcMessage message(OUTGOING_IRC_MSG, NAMES, 0, 0, 0, 0, _name);
      SendMessage(_server, &message);
    }
  } else if (!strncmp(cmd, "/leave", 6)) {
    DoLeave();
  } else if (!strncmp(cmd, "/msg ", 5)) {
    if (strlen(&cmd[5]) > 0)
      _server->GetChannel(&cmd[5])->MapRaised();
  } else if (!strncmp(cmd, "/nick ", 6)) {
    if (strlen(&cmd[6]) > 0)
      _server->ChangeNick(&cmd[6]);
  } else if (!strncmp(cmd, "/raw ", 5)) {
    if (strlen(&cmd[5]) > 0)
      _server->SendRawCommand(&cmd[5]);
  } else if (!strncmp(cmd, "/quote ", 7)) {
    if (strlen(&cmd[7]) > 0)
      _server->SendRawCommand(&cmd[7]);
  } else if (!strncmp(cmd, "/wallops ", 9)) {
    if (strlen(&cmd[9]) > 0)
      _server->SendWallops(&cmd[9]);
  } else if (!strncmp(cmd, "/mode ", 6)) {
    if (strlen(&cmd[6]) > 0)
      _server->SendMode(_name,&cmd[6]);
  } else if (!strncmp(cmd, "/umode ", 7)) {
    if (strlen(&cmd[7]) > 0)
      _server->SendUMode(&cmd[7]);
  } else if (!strncmp(cmd, "/whois ", 7)) {
    if (strlen(&cmd[7]) > 0)
      _server->SendRawCommand(&cmd[1]);
  } else if (!strncmp(cmd, "/whowas ", 8)) {
    if (strlen(&cmd[8]) > 0)
      _server->SendRawCommand(&cmd[1]);
  } else if (!strncmp(cmd, "/invite ", 8)) {
    if (strlen(&cmd[8]) > 0) {
      sprintf(char1, "INVITE %s %s", &cmd[8], _name);
      _server->SendRawCommand(char1);
    }
  } else {
    if (strlen(cmd) > 1) _server->SendRawCommand(&cmd[1]);
//    char *p = strchr(cmd, ' ');
//    if (p) *p = '\0';
//    sprintf(char1, "Unknown command: %s", cmd);
//    Log(char1, "red");
  }
  return True;
}

//----------------------------------------------------------------------

void OXChatChannel::SetChannelMode(char *mode_str) {
  int i, bit;
  unsigned long mode_bits = 0L;

  if (!mode_str) return;

  for (i=0; i<strlen(mode_str); ++i) {
    bit = mode_str[i] - 'a';  // use tolower here?
    if (bit >= 0 && bit <= 26) mode_bits |= (1 << bit);
  }
  SetChannelMode(mode_bits);
}

//----------------------------------------------------------------------

void OXChatChannel::SetChannelMode(unsigned long mode_bits) {
  if (_cmode != mode_bits) {
    _cmode = mode_bits;
    if (_ecmode & CMODE_SETBAN)     _menumode->CheckEntry(M_BAN);
    else                            _menumode->UnCheckEntry(M_BAN);
    if (_ecmode & CMODE_INVITEONLY) _menumode->CheckEntry(M_INVITEONLY);
    else                            _menumode->UnCheckEntry(M_INVITEONLY);
    if (_ecmode & CMODE_SETKEY)     _menumode->CheckEntry(M_KEY);
    else                            _menumode->UnCheckEntry(M_KEY);
    if (_ecmode & CMODE_SETULIMIT)  _menumode->CheckEntry(M_LIMIT);
    else                            _menumode->UnCheckEntry(M_LIMIT);
    if (_ecmode & CMODE_MODERATED)  _menumode->CheckEntry(M_MODERATED);
    else                            _menumode->UnCheckEntry(M_MODERATED);
    if (_ecmode & CMODE_NOMSGS)     _menumode->CheckEntry(M_QUIET);
    else                            _menumode->UnCheckEntry(M_QUIET);
//  if (_ecmode & CMODE_SETCHANOP)  _menumode->CheckEntry(??);
//  else                            _menumode->UnCheckEntry(??);
    if (_ecmode & CMODE_PRIVATE)    _menumode->CheckEntry(M_PRIVATE);
    else                            _menumode->UnCheckEntry(M_PRIVATE);
    if (_ecmode & CMODE_SECRET)     _menumode->CheckEntry(M_SECRET);
    else                            _menumode->UnCheckEntry(M_SECRET);
    if (_ecmode & CMODE_OPTOPIC)    _menumode->CheckEntry(M_TOPIC);
    else                            _menumode->UnCheckEntry(M_TOPIC);
//  if (_ecmode & CMODE_SETSPEAK)   _menumode->CheckEntry(??);
//  else                            _menumode->UnCheckEntry(??);
  }
}

//----------------------------------------------------------------------

void OXChatChannel::SetUserMode(unsigned long mode_bits) {
  _umode = mode_bits;
  // update menu...
}

//----------------------------------------------------------------------

void OXChatChannel::EnableChannelMode(char *mode_str) {
  int i, bit;
  unsigned long mode_bits = 0L;

  if (!mode_str) return;

  for (i=0; i<strlen(mode_str); ++i) {
    bit = mode_str[i] - 'a';  // use tolower here?
    if (bit >= 0 && bit <= 26) mode_bits |= (1 << bit);
  }
  SetChannelMode(mode_bits);
}

//----------------------------------------------------------------------

void OXChatChannel::EnableChannelMode(unsigned long mode_bits) {
  if (_ecmode != mode_bits) {
    _ecmode = mode_bits;
    if (_ecmode & CMODE_SETBAN)     _menumode->EnableEntry(M_BAN);
    if (_ecmode & CMODE_INVITEONLY) _menumode->EnableEntry(M_INVITEONLY);
    if (_ecmode & CMODE_SETKEY)     _menumode->EnableEntry(M_KEY);
    if (_ecmode & CMODE_SETULIMIT)  _menumode->EnableEntry(M_LIMIT);
    if (_ecmode & CMODE_MODERATED)  _menumode->EnableEntry(M_MODERATED);
    if (_ecmode & CMODE_NOMSGS)     _menumode->EnableEntry(M_QUIET);
//  if (_ecmode & CMODE_SETCHANOP)  _menumode->EnableEntry(??);
    if (_ecmode & CMODE_PRIVATE)    _menumode->EnableEntry(M_PRIVATE);
    if (_ecmode & CMODE_SECRET)     _menumode->EnableEntry(M_SECRET);
    if (_ecmode & CMODE_OPTOPIC)    _menumode->EnableEntry(M_TOPIC);
//  if (_ecmode & CMODE_SETSPEAK)   _menumode->EnableEntry(??);
  }
}

//----------------------------------------------------------------------

void OXChatChannel::EnableUserMode(unsigned long mode_bits) {
  _eumode = mode_bits;
  // update menu...
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++==

void OXChatChannel::Log(char *message) {
  Log(message, 11);
}

//----------------------------------------------------------------------

void OXChatChannel::Log(char *message, char *color) {
  OXChannel::Log(message, color);
  if (_logfile) {
    fprintf(_logfile, "%s\n", message);
    _menulog->EnableEntry(L_FLUSH);  // flush
  }
}

//----------------------------------------------------------------------

void OXChatChannel::Log(char *message, int color) {
  OXChannel::Log(message, color);
  if (_logfile) {
    fprintf(_logfile, "%s\n", message);
    _menulog->EnableEntry(L_FLUSH);  // flush
  }
}

//----------------------------------------------------------------------

void OXChatChannel::DoOpenLog() {
  OFileInfo fi;

  if (_logfile) DoCloseLog();  // perhaps we should ask first...

  // always open the file in append mode,
  // perhaps we should ask for this too...

  fi.MimeTypesList = NULL;
  fi.file_types = filetypes;
  new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &fi);
  if (fi.filename) {
    _logfilename = new char[strlen(fi.filename)+1];
    strcpy(_logfilename, fi.filename);
    _logfile = fopen(_logfilename, "a");
    if (!_logfile) {
      OString stitle("File Open Error");
      OString smsg("Failed to open log file: ");
      smsg.Append(strerror(errno));
      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                   MB_ICONSTOP, ID_OK);
      delete[] _logfilename;
    }
    time_t now = time(NULL);
    fprintf(_logfile, "****** IRC log started %s", ctime(&now));
    _menulog->EnableEntry(L_FLUSH);
    _menulog->EnableEntry(L_CLOSE);
    _menulog->EnableEntry(L_EMPTY);
  }
}

//----------------------------------------------------------------------

void OXChatChannel::DoCloseLog() {
  if (_logfile) {
    time_t now = time(NULL);
    fprintf(_logfile, "****** IRC log ended %s", ctime(&now));
    fclose(_logfile);
    _logfile = NULL;
    delete[] _logfile;
    _logfile = NULL;
    _menulog->DisableEntry(L_FLUSH);
    _menulog->DisableEntry(L_CLOSE);
    _menulog->DisableEntry(L_EMPTY);
  }
}

//----------------------------------------------------------------------

void OXChatChannel::DoFlushLog() {
  if (_logfile) {
    fflush(_logfile);
    _menulog->DisableEntry(L_FLUSH);
  }
}

//----------------------------------------------------------------------

void OXChatChannel::DoEmptyLog() {
}

//----------------------------------------------------------------------

void OXChatChannel::DoPrintLog() {
}

//----------------------------------------------------------------------

void OXChatChannel::DoToggleToolBar() {
}

//----------------------------------------------------------------------

void OXChatChannel::DoToggleStatusBar() {
  if (_statusBar->IsVisible()) {
    HideFrame(_statusBar);
    _menuview->UnCheckEntry(802);
  } else {
    ShowFrame(_statusBar);
    _menuview->CheckEntry(802);
  }
}

//----------------------------------------------------------------------

void OXChatChannel::DoToggleTopicBar() {
  if (_ftopic->IsVisible()) {
    HideFrame(_ftopic);
    HideFrame(_topicsep);
    _menuview->UnCheckEntry(803);
  } else {
    ShowFrame(_ftopic);
    ShowFrame(_topicsep);
    _menuview->CheckEntry(803);
  }
}

//----------------------------------------------------------------------

int OXChatChannel::DoLeave() {
  OString *lmsg = new OString(_name);

  if (foxircSettings->Confirm(P_CONFIRM_LEAVE) && _server->Connected()) {
    int retc;
    OString *msg = new OString("");
    OString *title = new OString("Leaving "); title->Append(_name);
    OString *text = new OString("Really leave channel ");
             text->Append(_name); text->Append("?");

    new OXConfirmDlg(_client->GetRoot(), this, title, text, msg, &retc);
    if (retc == ID_NO) return False;
    if (msg->GetLength() > 0) { lmsg->Append(" :"); lmsg->Append(msg); }
  }
  OIrcMessage message(OUTGOING_IRC_MSG, LEAVE, 0, 0, 0, 0,
                      (char *) lmsg->GetString());
  SendMessage(_server, &message);
  return OXChannel::CloseWindow();
}

//----------------------------------------------------------------------

void OXChatChannel::DoChannelMode(const char *mode) {
TDDLList<OString *> *sl = new TDDLList<OString *>();
char *temp = StrDup(mode);
const char*mode_str;
  int i, bit,par;
  bool add=true;
  par = 2;

char *ptr = strtok(temp, " ");
while(ptr){
	if(ptr)
		sl->Add(new OString(ptr));
		ptr = strtok(0," ");
	}
if(!sl->GetSize()>0)
	{
	delete []temp;
	delete sl;
	return;
	}
mode_str = sl->GetAt(1)->GetString();

  for (i=0; i<strlen(mode_str); ++i) {
    bit = mode_str[i];
    if(bit =='+')
    	add=true;
    else if(bit == '-')
    	add=false;
    else {
    switch(bit) {
	case 'i':
		printf("%s Invite only\n",add ? "Adding" : "Removing");
		break;
	case 't':
		printf("%s Topic\n",add ? "Adding" : "Removing");
		break;
	case 'm':
		printf("%s Moderated\n",add ? "Adding" : "Removing");
		break;
	case 'n':
		printf("%s No Msg\n",add ? "Adding" : "Removing");
		break;
	case 's':
		printf("%s Secret\n",add ? "Adding" : "Removing");
		break;
	case 'p':
		printf("%s Private\n",add ? "Adding" : "Removing");
		break;
	case 'K':
		printf("%s Knock\n",add ? "Adding" : "Removing");
		break;
	case 'o':
		{
		const char *who = sl->GetAt(par++)->GetString();
		printf("%s op %s\n",add ? "Adding" : "Removing",who);
		if(add)
			_nlist->OpNick(who);
		else 
			_nlist->NormalNick(who);
		}
		break;
	case 'v':
		{
		const char *who = sl->GetAt(par++)->GetString();
		printf("%s voice %s\n",add ? "Adding" : "Removing",who);
		if(add)
			_nlist->VoiceNick(who);
		else
			_nlist->NormalNick(who);
		}
		break;
	case 'l':
		printf("%s Limit %s\n",add ? "Adding" : "Removing",
			add ? sl->GetAt(par++)->GetString() : "");
		break;
	case 'b':
		printf("%s Ban %s\n",add ? "Adding" : "Removing",sl->GetAt(par++)->GetString());
		break;
	case 'k':
		printf("%s Key %s\n",add ? "Adding" : "Removing",sl->GetAt(par++)->GetString());
		break;
	default:
		break;
	}
    }
    
  }
delete []temp;
delete sl;

}
