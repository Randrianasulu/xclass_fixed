#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <X11/keysym.h>

#include "IRCcodes.h"
#include "OIrcMessage.h"
#include "OXChannel.h"
#include "OXChatChannel.h"
#include "OXMsgChannel.h"
#include "OXDCCChannel.h"
#include "OXPreferences.h"
#include "OXIrc.h"
#include "OXServerDlg.h"
#include "OXConfirmDlg.h"
#include "OXChannelDialog.h"

#include "pixmaps/tb-open.xpm"
#include "pixmaps/tb-save.xpm"
#include "pixmaps/tb-print.xpm"
#include "pixmaps/tb-setup.xpm"
#include "pixmaps/tb-conn.xpm"
#include "pixmaps/tb-dconn.xpm"

SToolBarData tdata[] = {
  { "tb-open.xpm",   tb_open,  "Open Log",          BUTTON_NORMAL, 801,  NULL },
  { "tb-save.xpm",   tb_save,  "Flush Log",         BUTTON_NORMAL, 802,  NULL },
  { "tb-print.xpm",  tb_print, "Print Log",         BUTTON_NORMAL, 803,  NULL },
  { "",              NULL,     0,                   0,             -1,   NULL },
  { "tb-conn.xpm",   tb_conn,  "Connect to Server", BUTTON_NORMAL, 1001, NULL },
  { "tb-dconn.xpm",  tb_dconn, "Close connection",  BUTTON_NORMAL, 1002, NULL },
  { "",              NULL,     0,                   0,             -1,   NULL },
  { "tb-setup.xpm",  tb_setup, "Setup Preferences", BUTTON_NORMAL, 2005, NULL },
  { NULL,            NULL,     NULL,                0,             0,    NULL }
};

SPopupData plogdata = {
  { NULL }, {
  { "&Open...", 801,             0, NULL },
  { "&Flush",   802, MENU_DISABLED, NULL },
  { "&Close",   804, MENU_DISABLED, NULL },
  { "",          -1,            -1, NULL },
  { "&Empty",   805, MENU_DISABLED, NULL },
  { NULL,        -1,            -1, NULL } }
};

SPopupData pservdata = {
  { NULL }, {
  { "&Raw...",      5001,          0, NULL },
  { "&Nick...",     5002,          0, NULL },
  { "&Wallops...",  5003,          0, NULL },
  { "&Links...",    1,             0, NULL },
  { "&Version...",  1,             0, NULL },
  { "&Motd",        5006,          0, NULL },
  { "&Time...",     1,             0, NULL },
  { "T&race...",    1,             0, NULL },
  { "&Admin...",    1, MENU_DISABLED, NULL },
  { "L&users...",   1,             0, NULL },
  { "&Info...",     1,             0, NULL },
  { "&Stats...",    1,             0, NULL },
  { "&Oper...",     1, MENU_DISABLED, NULL },
  { "Re&hash",      1, MENU_DISABLED, NULL },
  { "R&estart",     1, MENU_DISABLED, NULL },
  { "S&quit",       1, MENU_DISABLED, NULL },
  { NULL,          -1,            -1, NULL } }
};

SPopupData pusersdata = {
  { NULL }, {
  { "W&ho",      1,  0, NULL },
  { "&Whois",    1,  0, NULL },
  { "Whow&as",   1,  0, NULL },
  { "&Mode",     1,  0, NULL },
  { "&CTCP",     1,  0, NULL },
  { "&DCC",      1,  0, NULL },
  { "&Invite",   1,  0, NULL },
  { "M&sg",      1,  0, NULL },
  { "&Notice",   1,  0, NULL },
  { "&Finger",   1,  0, NULL },
  { "&Time",     1,  0, NULL },
  { "T&race",    1,  0, NULL },
  { "&Userhost", 1,  0, NULL },
  { "&Kill",     1,  0, NULL },
  { NULL,       -1, -1, NULL } }
};

SPopupData pchandata = {
  { NULL }, {
  { "&Favourites...",  1,  0, NULL },
  { "",               -1,  0, NULL },
  { "&Join...",        5000,  0, NULL },
  { "&Who...",         1,  0, NULL },
  { "&List...",        1,  0, NULL },
  { "&Names...",       1,  0, NULL },
  { "N&otice...",      1,  0, NULL },
  { "&Monitor...",     1,  0, NULL },
  { "&CTCP...",        1,  0, NULL },
  { NULL,          -1, -1, NULL } }
};

SPopupData mircdata = {
  { NULL }, {
  { "&Connect to...", 1001,              0, NULL },
  { "&Disconnect",    1002,              0, NULL },
  { "&Away",          1003,  MENU_DISABLED, NULL },
  { "&Brb",           1004,  MENU_DISABLED, NULL },
  { "",                 -1,              0, NULL },
  { "&Log",           1005,              0, &plogdata   },
  { "&Servers",       1006,              0, &pservdata  },
  { "&Users",         1007,              0, &pusersdata },
  { "C&hannels",      1008,              0, &pchandata  },
  { "",                 -1,              0, NULL },
  { "E&xit",          1009,              0, NULL },
  { NULL,               -1,             -1, NULL } }
};

SPopupData meditdata = {
  { NULL }, {
  { "Cu&t",              2001, MENU_DISABLED, NULL },
  { "&Copy",             2002, MENU_DISABLED, NULL },
  { "",                    -1,             0, NULL },
  { "Select &All",       2003, MENU_DISABLED, NULL },
  { "&Invert Selection", 2004, MENU_DISABLED, NULL },
  { "",                    -1,             0, NULL },
  { "&Preferences...",   2005,             0, NULL },
  { NULL,                  -1,            -1, NULL } }
};

SPopupData mviewdata = {
  { NULL }, {
  { "&Toolbar",    3001,  MENU_CHECKED, NULL },
  { "Status &Bar", 3002,  MENU_CHECKED, NULL },
  { "",              -1,             0, NULL },
  { "&Fonts...",   3003, MENU_DISABLED, NULL },
  { "&Colors...",  3004, 	     0, NULL },
  { NULL,            -1,            -1, NULL } }
};

SPopupData mhelpdata = {
  { NULL }, {
  { "&Contents...", 4001, MENU_DISABLED, NULL },
  { "&Search...",   4002, MENU_DISABLED, NULL },
  { "",               -1,             0, NULL },
  { "&About...",    4003,             0, NULL },
  { NULL,             -1,            -1, NULL } }
};


//----------------------------------------------------------------------

OLayoutHints *leftexpandylayout =
    new OLayoutHints(LHINTS_LEFT | LHINTS_EXPAND_Y, 3, 0, 0, 0);

OLayoutHints *leftcenterylayout =
    new OLayoutHints(LHINTS_LEFT | LHINTS_CENTER_Y);

OLayoutHints *topleftlayout =
    new OLayoutHints(LHINTS_TOP | LHINTS_LEFT, 1, 1, 1, 1);

OLayoutHints *topexpandxlayout =
    new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X, 0, 0, 0, 3);

OLayoutHints *topexpandxlayout1 =
    new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X, 0, 0, 3, 0);

OLayoutHints *topexpandxlayout2 =
    new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X, 0, 0, 3, 3);

OLayoutHints *topexpandxlayout3 =
    new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X);

OLayoutHints *topexpandxlayout0 =
    new OLayoutHints(LHINTS_TOP | LHINTS_EXPAND_X, 1, 1, 0, 0);

OLayoutHints *expandxexpandylayout =
    new OLayoutHints(LHINTS_EXPAND_Y | LHINTS_EXPAND_X);

OLayoutHints *menubarlayout =
    new OLayoutHints(LHINTS_TOP | LHINTS_LEFT | LHINTS_EXPAND_X, 0, 0, 1, 1);

OLayoutHints *menuitemlayout =
    new OLayoutHints(LHINTS_TOP | LHINTS_LEFT, 0, 4, 0, 0);

unsigned long IRCcolors[16];
OGC *_ircGCs[16];

extern OSettings  *foxircSettings;

char *filetypes[] = { "All files",  "*",
                      "Log files",  "*.log",
                      "Text files", "*.txt",
                      NULL,         NULL };

OXIrc::OXIrc(const OXWindow *p, char *nick, char *server, int port) :
  OXMainFrame(p, 400, 300) {

  channels = NULL;
  _logfile = NULL;
  _logfilename = NULL;
  _fh = NULL;
  _init = False;

  _avcmode = 0x00444100L; // 0L;
  _avumode = 0x00088100L; // 0L;

  // color table, similar to the one used by mIRC and cIRCus
/* 
  //   This is the old table using the color's name
  //   Which didn't seem to match the mirc colors very well
  //   So I've updated the table to use the RRGGBB color form
  //    Mike

  IRCcolors[0]  = _client->GetColorByName("white");
  IRCcolors[1]  = _client->GetColorByName("black");
  IRCcolors[2]  = _client->GetColorByName("royalblue");
  IRCcolors[3]  = _client->GetColorByName("darkgreen");
  IRCcolors[4]  = _client->GetColorByName("red3");
  IRCcolors[5]  = _client->GetColorByName("brown");
  IRCcolors[6]  = _client->GetColorByName("mediumpurple");
  IRCcolors[7]  = _client->GetColorByName("orange");
  IRCcolors[8]  = _client->GetColorByName("yellow");
  IRCcolors[9]  = _client->GetColorByName("lightgreen");
  IRCcolors[10] = _client->GetColorByName("cyan");
  IRCcolors[11] = _client->GetColorByName("lightcyan");
  IRCcolors[12] = _client->GetColorByName("lightblue");
  IRCcolors[13] = _client->GetColorByName("pink");
  IRCcolors[14] = _client->GetColorByName("darkslategray");
  IRCcolors[15] = _client->GetColorByName("lightgray");
*/
  IRCcolors[0]  = _client->GetColorByName("white");
  _ircGCs[0] = new OGC(_client,"white");
  IRCcolors[1]  = _client->GetColorByName("black");
  _ircGCs[1] = new OGC(_client,"black");
  IRCcolors[2]  = _client->GetColorByName("#00007F");
  _ircGCs[2] = new OGC(_client,"#00007F");
  IRCcolors[3]  = _client->GetColorByName("#007F00");
  _ircGCs[3] = new OGC(_client,"#007F00");
  IRCcolors[4]  = _client->GetColorByName("#FF0000");
  _ircGCs[4] = new OGC(_client,"#FF0000");
  IRCcolors[5]  = _client->GetColorByName("#7F0000");
  _ircGCs[5] = new OGC(_client,"#7F0000");
  IRCcolors[6]  = _client->GetColorByName("#7F007F");
  _ircGCs[6] = new OGC(_client,"#7F007F");
  IRCcolors[7]  = _client->GetColorByName("#FF7F00");
  _ircGCs[7] = new OGC(_client,"#FF7F00");
  IRCcolors[8]  = _client->GetColorByName("#FFFF00");
  _ircGCs[8] = new OGC(_client,"#FFFF00");
  IRCcolors[9]  = _client->GetColorByName("#00FF00");
  _ircGCs[9] = new OGC(_client,"#00FF00");
  IRCcolors[10] = _client->GetColorByName("#007F7F");
  _ircGCs[10] = new OGC(_client,"#007F7F");
  IRCcolors[11] = _client->GetColorByName("#00FFFF");
  _ircGCs[11] = new OGC(_client,"#00FFFF");
  IRCcolors[12] = _client->GetColorByName("#0000FF");
  _ircGCs[12] = new OGC(_client,"#0000FF");
  IRCcolors[13] = _client->GetColorByName("#FF00FF");
  _ircGCs[13] = new OGC(_client,"#FF00FF");
  IRCcolors[14] = _client->GetColorByName("#7F7F7F");
  _ircGCs[14] = new OGC(_client,"#7F7F7F");
  IRCcolors[15] = _client->GetColorByName("#CFCFCF");
  _ircGCs[15] = new OGC(_client,"#CFCFCF");


  SetLayoutManager(new OVerticalLayout(this));

  //---- menu bar

  _menubar = new OXMenuBar(this, 10, 10, HORIZONTAL_FRAME);
  AddFrame(_menubar, menubarlayout);

  _pserv  = MakePopup(&pservdata);
  _pusers = MakePopup(&pusersdata);
  _pchan  = MakePopup(&pchandata);
  _plog   = MakePopup(&plogdata);

  _mirc  = MakePopup(&mircdata);
  _medit = MakePopup(&meditdata);
  _mview = MakePopup(&mviewdata);
  _mhelp = MakePopup(&mhelpdata);

  _menubar->AddPopup(new OHotString("&IRC"),  _mirc,  menuitemlayout);
  _menubar->AddPopup(new OHotString("&Edit"), _medit, menuitemlayout);
  _menubar->AddPopup(new OHotString("&View"), _mview, menuitemlayout);
  _menubar->AddPopup(new OHotString("&Help"), _mhelp, menuitemlayout);

  //---- toolbar

  _toolBarSep = new OXHorizontal3dLine(this);

  _toolBar = new OXToolBar(this, 60, 20, HORIZONTAL_FRAME);
  _toolBar->AddButtons(tdata);

  AddFrame(_toolBarSep, topexpandxlayout3);
  AddFrame(_toolBar, topexpandxlayout2);

  //---- log window

  _channelentry = new OXTextEntry(this, new OTextBuffer(100));
  _channelentry->ChangeOptions(FIXED_WIDTH | SUNKEN_FRAME | DOUBLE_BORDER);
  AddFrame(_channelentry, topexpandxlayout);

  _logw = new OXViewDoc(this, _log = new OTextDoc(), 10, 10,
                              SUNKEN_FRAME | DOUBLE_BORDER);
  _logw->SetScrollOptions(FORCE_VSCROLL | NO_HSCROLL);
  AddFrame(_logw, expandxexpandylayout);

  //------ status bar

  _statusBar = new OXStatusBar(this);
  AddFrame(_statusBar, new OLayoutHints(LHINTS_BOTTOM | LHINTS_EXPAND_X,
                               0, 0, 3, 0));


  _mview->CheckEntry(3001);
  _mview->CheckEntry(3002);

  SetWindowName("fOXIrc");
  SetClassHints("fOXIrc", "foxirc");

  MapSubwindows();
//  Resize(512, 320);
  Resize(600, 380);

  if (getenv("USER"))
    strcpy(_username, getenv("USER"));
  else if (getenv("LOGNAME"))
    strcpy(_username, getenv("LOGNAME"));
  else
    strcpy(_username, "unknown");

  if (nick && *nick)
    strcpy(_nick, nick);
  else
    strcpy(_nick, _username);

  if (getenv("HOSTNAME"))
    strcpy(_hostname, getenv("HOSTNAME"));
  else
    strcpy(_hostname, "unknown.host");

  if (server)
    strcpy(_server, server);
  else
    strcpy(_server, "");

  if (getenv("LOGNAME"))
    strcpy(_ircname, getenv("LOGNAME"));
  else
    strcpy(_ircname, _username);

  _port = port;

  _irc = new OIrc();
  _irc->Associate(this);

  _connected = False;

  _statusBar->SetWidth(0, 250);
  _statusBar->AddLabel(100, LHINTS_RIGHT);
  _statusBar->AddLabel(500);

  _statusBar->SetText(0, new OString("Not connected."));
  _statusBar->SetText(1, new OString("Lag: 0 secs"));
  _statusBar->SetText(2, new OString(nick));

}

OXIrc::~OXIrc() {
  _irc->Close();
  if (_logfile) DoCloseLog();
}

int OXIrc::CloseWindow() {
  OString *lmsg = new OString("QUIT");

  if (_connected && foxircSettings->Confirm(P_CONFIRM_LEAVE)) {
    int retc;
    OString *msg = new OString("");
    OString *title = new OString("Quit IRC?");
    OString *text = new OString("Really Quit fOXirc?");

    new OXConfirmDlg(_client->GetRoot(), this, title, text, msg, &retc);
    if (retc == ID_NO) return False;
    if (msg->GetLength() > 0) {
      lmsg->Append(" :");
      lmsg->Append(msg);
    } else {
      lmsg->Append(" : fOXIrc http://www.foxproject.org");   // hmmm...
    }
  } else {
    lmsg->Append(" : fOXIrc http://www.foxproject.org");   // hmmm...
  }

  OIrcMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0,
                      (char *) lmsg->GetString());
  SendMessage(_irc, &message);
//  sleep(2);
  Disconnect();
  while (channels) channels->CloseWindow();

  return OXMainFrame::CloseWindow();
}

OXPopupMenu *OXIrc::MakePopup(SPopupData *p) {

  OXPopupMenu *popup = new OXPopupMenu(_client->GetRoot());

  for (int i=0; p->popup[i].name != NULL; ++i) {
    if (strlen(p->popup[i].name) == 0) {
      popup->AddSeparator();
    } else {
      if (p->popup[i].popup_ref == NULL) {
        popup->AddEntry(new OHotString(p->popup[i].name), p->popup[i].id);
      } else {
        SPopupData *p1 = p->popup[i].popup_ref;
        popup->AddPopup(new OHotString(p->popup[i].name), p1->ptr);
      }
      if (p->popup[i].state & MENU_DISABLED)
        popup->DisableEntry(p->popup[i].id);
      if (p->popup[i].state & MENU_CHECKED)
        popup->CheckEntry(p->popup[i].id);
      if (p->popup[i].state & MENU_RCHECKED)
        popup->RCheckEntry(p->popup[i].id,
                           p->popup[i].id,
                           p->popup[i].id);
    }
  }
  p->ptr = popup;
  popup->Associate(this);

  return popup;
}

void OXIrc::Log(const char *message) {
  Log(message, 11);
}

void OXIrc::Log(const char *message, char *color) {
  OLineDoc *l = new OLineDoc();
  l->SetCanvas(_log->_style_server);
  l->SetColor(color);
  l->Fill((char*)message);
  _log->AddLine(l);
  _logw->ScrollUp();
  if (_logfile) {
    fprintf(_logfile, "%s\n", message);
    _plog->EnableEntry(802);  // flush
  }
}
void OXIrc::Log(const char *message, int color) {
  OLineDoc *l = new OLineDoc();
  l->SetCanvas(_log->_style_server);
  l->SetColor(foxircSettings->GetColorID(color));
  l->Fill((char*)message);
  _log->AddLine(l);
  _logw->ScrollUp();
  if (_logfile) {
    fprintf(_logfile, "%s\n", message);
    _plog->EnableEntry(802);  // flush
  }
}

int OXIrc::HandleMapNotify(XMapEvent *event) {
  if (!_init) {
    if (_server && *_server && _nick && *_nick)
      Connect(_server, _port);
    else if (foxircSettings->CheckMisc(P_MISC_POPUP_SERV_CN))
      DoConnect();
    _init = True;
  }
  return True;
}

int OXIrc::Connect(char *server, int port) {
  if (server) {
    char tmp[256];
    int retc;

    if (_connected) Disconnect();

    strcpy(_server, server);
    _port = port;

    if ((retc = _irc->Connect(_server, _port)) < 0) {
      sprintf(tmp, "Connection to %s:%d failed (%s).",
                   _server, _port, strerror(errno));
      Log(tmp, 4);
      return retc;
    }

    sprintf(tmp, "Connecting to server %s, port %d", _server, _port);
    Log(tmp, 4);

    if (_fh) delete _fh;
    _fh = new OFileHandler(this, _irc->GetFD(), XCM_WRITABLE | XCM_EXCEPTION);
  }
  return -1;
}

int OXIrc::Disconnect() {
  _irc->Close();
  if (_connected) {
    _connected = False;
    if (_fh) delete _fh;
    _fh = NULL;
    Log("Connection closed.", 4);
    Log(" ", 4);
  }
  _statusBar->SetText(0, new OString("Not connected."));

  // should close all channels, ask for confirmation, etc...
  // (if connecting to a different server, shall we re-join all active
  // channels?)
  return 0;
}

int OXIrc::HandleFileEvent(OFileHandler *fh, unsigned int mask) {
  if (!_connected) {
    char tmp[256];

    _connected = True;

    sprintf(tmp, "Connected to %s:%d", _server, _port);
    _statusBar->SetText(0, new OString(tmp));
    _statusBar->SetText(2, new OString(_nick));

    if (_fh) delete _fh;
    _fh = new OFileHandler(this, _irc->GetFD(), XCM_READABLE);

    Log("Connected, logging in...", 4);
    OIrcMessage msg1(OUTGOING_IRC_MSG, NICK, 0, 0, 0, 0, _nick);
    OIrcMessage msg2(OUTGOING_IRC_MSG, USER, 0, 0, 0, 0,
                     _username, _hostname, _server, _ircname);
    SendMessage(_irc, &msg1);
    SendMessage(_irc, &msg2);
    //Log(" ");

    if (foxircSettings->CheckMisc(P_MISC_POPUP_CHAN_CN))
      new OXChannelDialog(_client->GetRoot(), this);

  } else {
    _irc->Receive();
  }

  return True;
}


// Find a channel window with the given name, if not found create it.

OXChannel *OXIrc::GetChannel(const char *channel) {
  OXChannel *ch;
  const OXWindow *main;

  if ((ch = FindChannel(channel)) != NULL) return ch;

  if (foxircSettings->CheckChannelFlags(channel,TRANSIENT_WINDOW))
    main = this;
  else
    main = NULL;

  if (channel && *channel == '#') {
    ch = new OXChatChannel(_client->GetRoot(), main, this, channel);
    {
      OIrcMessage message(OUTGOING_IRC_MSG, NAMES, 0, 0, 0, 0, (char *)channel);
      SendMessage(this, &message);
    }
    ((OXChatChannel *) ch)->EnableChannelMode(_avcmode);
  } else if (channel && *channel == '=') {
    ch = new OXDCCChannel(_client->GetRoot(), main, this, channel);
  } else {
    ch = new OXMsgChannel(_client->GetRoot(), main, this, channel);
  }

  // new channels are added to the beginning of the list...

  ch->_next = channels;
  if (ch->_next) ch->_next->_prev = ch;
  ch->_prev = NULL;
  channels = ch;
  if(ch->_cinfo->background)
	  ch->_logw->SetupBackgroundPic(ch->_cinfo->background);
  return ch;
}


// Find a channel window with the given name, if not found return NULL.

OXChannel *OXIrc::FindChannel(const char *channel) {
  OXChannel *ch;

  for (ch = channels; ch; ch = ch->_next) {
    if (strcasecmp(channel, ch->_name) == 0) return ch;
  }

  return NULL;
}

void OXIrc::RemoveChannel(OXChannel *ch) {
  if (channels == ch) channels = ch->_next;
  if (ch->_next) ch->_next->_prev = ch->_prev;
  if (ch->_prev) ch->_prev->_next = ch->_next;
}

int OXIrc::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;
  char char1[TCP_BUFFER_LENGTH], char2[TCP_BUFFER_LENGTH],
       char3[TCP_BUFFER_LENGTH], char4[TCP_BUFFER_LENGTH],
       char5[TCP_BUFFER_LENGTH], char6[TCP_BUFFER_LENGTH],
       char7[TCP_BUFFER_LENGTH], char8[TCP_BUFFER_LENGTH];

  int  int1, int2, int3;
  time_t tm, tm2;

  switch(msg->type) {
    case MSG_BUTTON:
    case MSG_MENU:
      switch (msg->action) {
        case MSG_CLICK:
          switch(wmsg->id) {
            case 801: DoOpenLog();  break;
            case 802: DoFlushLog(); break;
            case 804: DoCloseLog(); break;
            case 805: DoEmptyLog(); break;

            case 1001:
              DoConnect();
              break;

            case 1002:
              Disconnect();
              break;

            case 1009:
              CloseWindow();
              delete _client;
              break;

            case 2005:
              new OXPreferencesDialog(_client->GetRoot(), this, foxircSettings);
              break;

            case 3001:
              DoToggleToolBar();
              break;

            case 3002:
              DoToggleStatusBar();
              break;

	    case 3004:
		{
//		static int s = 0;
//	    	new OX16ColorDialog(_client->GetRoot(),this,&s);
//		printf("S is %d\n",s);
		}
		break;
            case 4003:
              DoHelpAbout();
              break;

            case 5000:
              new OXChannelDialog(_client->GetRoot(), this);
	      break;
	      
            case 5001:
              DoRaw();
              break;

            case 5002:
              DoNick();
              break;

            case 5003:
              DoWallops();
              break;

            case 5006:
              DoMotd();
              break;

          }
        break;
      }
      break;

    case MSG_TEXTENTRY:
      {
        OTextEntryMessage *tmsg = (OTextEntryMessage *) msg;
        switch(msg->action) {
          case MSG_TEXTCHANGED:
            switch(tmsg->keysym) {
              case XK_Return:
                strcpy(char1, _channelentry->GetString());
                if (strlen(char1) > 0) {
                  OIrcMessage message(OUTGOING_IRC_MSG, JOIN, 0, 0, 0, 0, char1);
                  SendMessage(this, &message);
                }
	        _channelentry->Clear();
                break;
            }
            break;
        }
      }
      break;

    case OUTGOING_TCP_MSG:
      SendMessage(_irc, msg);
      break;

    case INCOMING_IRC_MSG:
      {
        OIrcMessage *ircmsg = (OIrcMessage *) msg;

        switch(msg->action) {
          case AUTH:
            Log(ircmsg->string3->GetString(), 5);
            break;

          case PRIVMSG:
          case NOTICE:
            if (!strcasecmp(ircmsg->string1->GetString(), _nick)) {
              GetChannel(ircmsg->string2->GetString())
                             ->Say((char *)ircmsg->string2->GetString(),
                                   (char *)ircmsg->string3->GetString(),
                                   msg->action);
            } else {
              GetChannel(ircmsg->string1->GetString())
                             ->Say((char *)ircmsg->string2->GetString(),
                                   (char *)ircmsg->string3->GetString(),
                                   msg->action);
            }
	    break;

          case MODE:
            if (strcmp(ircmsg->string1->GetString(), _nick) == 0) {
              sprintf(char6, "Mode change \"%s\" for user %s by %s", 
                             ircmsg->string3->GetString(),
                             ircmsg->string1->GetString(),
                             ircmsg->string2->GetString());
              Log(char6, 9);
            } else {
              // It is OK to pass a NULL ptr to SendMessage.
              SendMessage(FindChannel(ircmsg->string1->GetString()), msg);
            }
            break;

          case LEAVE:
          case KICK:
          case TOPIC_SET:
            SendMessage(FindChannel(ircmsg->string1->GetString()), msg);
	    break;

          case JOIN:
          case NICKLIST:
            SendMessage(GetChannel(ircmsg->string1->GetString()), msg);
	    break;

          case NICK:
          case QUIT:
            for(OXChannel *i = channels; i; i= i->_next)
              SendMessage(i, msg);  // OK to send it to all windows???
	    break;

          case SPECIAL:
            //char1[0] = '\0';
            //sscanf(char1, "%s", ircmsg->string4->GetString());
            switch (ircmsg->parm1) {
              case RPL_WELCOME:    // :Welcome...
              case RPL_YOURHOST:   // :Your host is...
              case RPL_CREATED:    // :This server was created...
              case RPL_PROTOCTL:   // 5
              case RPL_LUSERCLIENT:
              case RPL_LUSERME:
              case RPL_STATSCONN:  // :Highest connection count...
              case RPL_LOCALUSERS:
              case RPL_GLOBALUSERS:
              case RPL_WHOISMODES:
                Log(ircmsg->string4->GetString(), 5);
                break;

              case RPL_INVITING:
                if (sscanf(ircmsg->string4->GetString(), "%s %s", 
                           char1, char2) == 2) {
                  sprintf(char3, "Inviting %s to %s", char1, char2);
                  Log(char3, 6);
                }
                break;
		
              case 381: // :you are now an ircop..
              case 396: // :info set to
                Log(ircmsg->string4->GetString(), 12);
		break;

              case RPL_LUSEROP:
              case RPL_LUSERUNKNOWN:
              case RPL_LUSERCHANNELS:
                if (sscanf(ircmsg->string4->GetString(), "%d :", &int1) == 1) {
                  char *s = strchr(ircmsg->string4->GetString(), ':');
                  if (s) ++s;
                  sprintf(char6, "%d", int1);
                  if (s && *s) sprintf(char6, "%s %s", char6, s);
                  Log(char6, 5);
                } else {
                  Log(ircmsg->string4->GetString(), 5);
                }
                break;

              //--------------------------------- WHOIS responses

              case RPL_WHOISUSER:
              case RPL_WHOISSERVER:
              case RPL_WHOISCHANNELS:
              case RPL_WHOISIDLE:
              case RPL_ENDOFWHOIS:
              case RPL_ENDOFWHOWAS:
              case RPL_WHOISOPERATOR:
              case RPL_WHOISHELPOP:
              case RPL_WHOISSADMIN:
              case RPL_WHOISADMIN:
              case RPL_WHOISNETWORK:
              case RPL_WHOWASUSER:
              case 379: // : is a services admin
              case 378: // : Real Hostname :
                ProcessWhois(ircmsg->parm1, ircmsg->string4->GetString());
                break;

              case RPL_AWAY:  // this can be part of WHOIS response, but other
                              // can generate this response as well.
                if (sscanf(ircmsg->string4->GetString(), "%s :", char1) == 1) {
                  char *s = strchr(ircmsg->string4->GetString(), ':');
                  if (s) ++s;
                  sprintf(char6, "%s is away", char1);
                  if (s && *s) sprintf(char6, "%s (%s)", char6, s);
                  /*GetChannel(char1)->*/Log(char6, 5);
                }
                break;

              //--------------------------------- MOTD

              case RPL_MOTDSTART:
              case RPL_MOTD:
                Log(ircmsg->string4->GetString(), 4);
                break;
              case RPL_ENDOFMOTD:
                Log("End of MOTD", 5);
                break;
              case ERR_NOMOTD:
                sprintf(char6, "%s", 
                        (ircmsg->string4) ? 
                        ircmsg->string4->GetString() : "No MOTD");
                Log(char6, 5);
                break;

              //---------------------------------

              case RPL_MYINFO:  // 004 - host, version, umodes, cmodes
                {
                if (sscanf(ircmsg->string4->GetString(), "%s %s %s %s",
                           char1, char2, char3, char4) == 4) {
                  _avumode = ModeBits(char3);
                  _avcmode = ModeBits(char4);
                  sprintf(char1, "umodes available: %s, channel modes available: %s",
                                 char3, char4);
                  Log(char1, 5);
                  }
                }
                break;

              case RPL_CHANNELMODEIS:
                break;

              //--------------------------------- Names List

              case RPL_NAMREPLY:
	        {
                  OIrcMessage message(INCOMING_IRC_MSG, NICKLIST, 0, 0, 0, 0,
	                              (char*)(ircmsg->string3->GetString()),
				      (char*)(ircmsg->string4->GetString()));
                  ProcessMessage(&message);
	        }
              case RPL_ENDOFNAMES:
                break;

              //---------------------------------

              case RPL_CREATIONTIME:  // channel creation time
                if (sscanf(ircmsg->string4->GetString(), "%s %lu",
                           char1, &tm) == 2) {
                  OIrcMessage message(INCOMING_IRC_MSG, SPECIAL,
                                      ircmsg->parm1, (long) tm, 0, 0);
                  SendMessage(GetChannel(char1), &message);
                }
                break;

              case ERR_CHANOPRIVSNEEDED:
                if (sscanf(ircmsg->string4->GetString(), "%s :%n",
                           char1, &int1) == 1) {
                  OIrcMessage message(INCOMING_IRC_MSG, SPECIAL,
                                      ircmsg->parm1, 0, 0, 0);
                  SendMessage(GetChannel(char1), &message);
                }
                break;

              case ERR_NOSUCHNICK:
              case ERR_NOSUCHSERVER:
              case ERR_NOSUCHCHANNEL:
              case ERR_CANNOTSENDTOCHAN:
              case ERR_TOOMANYCHANNELS:
              case ERR_WASNOSUCHNICK:
              case ERR_TOOMANYTARGETS:
              case ERR_NOORIGIN:
              case 464: // :invalid password
              case 491: // :No O-lines for your host
                Log(ircmsg->string4->GetString(), 3);
                break;

              case 364: // link
                ProcessLink((char *) ircmsg->string4->GetString());
                break;

	      case 433: // :Nick is already in use..
                if (sscanf(ircmsg->string4->GetString(), "%s :", char1) == 1) {
                  sprintf(char6, "Nickname %s is already in use", char1);
                  Log(char6, 3);
                  OString *tmp = new OString("");
                  int retn;
                  sprintf(char6, "%s\nPlease enter another nick", char6);
                  new OXConfirmDlg(_client->GetRoot(), this,
               		new OString("Change Nick"), new OString(char6),
              	        tmp, &retn);
		  if (retn != ID_NO)
                    ChangeNick(tmp->GetString());
		  else
                    ChangeNick(_nick);
                  delete tmp;
                }
                break;

              //--------------------------------- TOPIC responses

              case RPL_TOPIC:
                if (sscanf(ircmsg->string4->GetString(), "%s %n",
                           char1, &int1) == 1) {
                  char *s = (char *) ircmsg->string4->GetString();
                  if (int1 < strlen(s)) {
                    if (s[int1] == ':') ++int1;
                    OIrcMessage message(INCOMING_IRC_MSG, TOPIC,
                                        0, 0, 0, 0, s+int1);
                    SendMessage(GetChannel(char1), &message);
                  }
                }
                break;

              case RPL_TOPICWHOTIME:
                if (sscanf(ircmsg->string4->GetString(), "%s %s %lu",
                           char1, char2, &tm) == 3) {
                  OIrcMessage message(INCOMING_IRC_MSG, TOPIC_SETBY,
                                      (long) tm, 0, 0, 0, char2);
                  SendMessage(GetChannel(char1), &message);
                }
                break;

              //--------------------------------- Misc error responses

              case ERR_UNKNOWNCOMMAND:
                strcpy(char6, "Unknown command");
                if (sscanf(ircmsg->string4->GetString(), "%s :", char1) == 1) {
                  sprintf(char6, "%s: %s", char6, char1);
                }
                Log(char6, 3);
                break;

              //--------------------------------- All others/unknown
//      //
//      //
//      //
//  //  //
//  //  //
 //////// 
 
//#fOX Zapper NetAdmin.FoxChat.Net roadkill.FoxChat.net Zapper G*@ :1 High Voltage
//#fOX Big_Mac foxproject.org Wild.FoxChat.Net Poki H :0 "Poki the wonderbot"
//* services foxchat.net services.foxchat.net ChanServ H* :3 Channel Server
//#fOX mike itchy2-16-70.ionsys.com Wild.FoxChat.Net Foxxer H :0 Me

	      case 352:   // Who reply. (channel, login, host, server, nick
		
		if(sscanf(ircmsg->string4->GetString(),"%s %s %s %s %s %s :%d",
		char1 , char2, char3 ,char4, char5,char6,&int1)==6){
		//chan login   host  server  nick  status
//		printf("found char1:%s, char2:%s, char3:%s, char4:%s, char5:%s, char5:%s\n",
//		char1,char2,char3,char4,char5,char6);
		sprintf(char7,"%64s %64s %s %s@%s [%d:%s]",
			char1,char5,char6,char2,char3,int1,char4);

		    char *s = strchr(ircmsg->string4->GetString(), ':');
		    if (s) ++s;
    			while ((s) && isdigit(s[0])) s++;

    		    if (s)
		    	sprintf(char7,"%s (%s)",char7,s);
            	    Log(char7, 3);
		}
		break;
              default:
                sprintf(char6, "Unknown SPECIAL INCOMING_IRC_MSG (%03d - %s)", 
                        ircmsg->parm1, (ircmsg->string4) ? 
                        ircmsg->string4->GetString() : "<null>");
                Log(char6, 3);
                break;
            }
	    break;

          case ERROR:
            Log(ircmsg->string1->GetString(), 3);
            break;

          case WALLOPS:
            sprintf(char6, "Wallops from %s [%s]", 
                        ircmsg->string1->GetString(), (ircmsg->string2) ? 
                        ircmsg->string2->GetString() : "<null>");
            Log(char6, 19);
            break;

          case INVITE:
            {
            int retval;

            sprintf(char6, "%s invites you to join channel %s\n"
                           "Do you want to join the channel?", 
                        ircmsg->string1->GetString(),
                        ircmsg->string3->GetString());
            new OXMsgBox(_client->GetRoot(), this,
                         new OString("Invite Dialog"), new OString(char6),
                         MB_ICONQUESTION, ID_YES|ID_IGNORE, &retval);
            if (retval == ID_YES) {
              OIrcMessage message(OUTGOING_IRC_MSG, JOIN, 0, 0, 0, 0,
                                 (char *) ircmsg->string3->GetString());
              SendMessage(this, &message);
            }
            }
            break;

          default:
	    printf("Incorrect/Unknown INCOMING_IRC_MESSAGE\n");
            break;
        }
      }
      break;

    case OUTGOING_IRC_MSG:
      {
        OIrcMessage *ircmsg = (OIrcMessage *) msg;

        switch(msg->action) {
          case JOIN:
            sprintf(char1, "JOIN %s\nMODE %s\n", 
	            ircmsg->string1->GetString(), 
		    ircmsg->string1->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_irc, &message);
	    }
	    break;

          case NAMES:
            sprintf(char1, "NAMES %s\n", ircmsg->string1->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_irc, &message);
	    }
	    break;

          case LEAVE:
            sprintf(char1, "PART %s\n", ircmsg->string1->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_irc, &message);
	    }
	    break;

          case TOPIC_SET:
            sprintf(char1, "TOPIC %s :%s\n",
                           ircmsg->string1->GetString(),
                           ircmsg->string2->GetString());
	    {
              OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, char1);
              SendMessage(_irc, &message);
	    }
	    break;

          default:
	    break;
	}
      }
      break;

    case BROKEN_PIPE:
      Log("Connection reset by peer", 4);
      Disconnect();
      break;

  }
  return True;
}

void OXIrc::ChangeNick(const char *nick) {
  strcpy(_nick, nick);
  OIrcMessage msg(OUTGOING_IRC_MSG, NICK, 0, 0, 0, 0, _nick);
  SendMessage(_irc, &msg);
  _statusBar->SetText(2, new OString(_nick));
}

void OXIrc::ProcessWhois(int cmd, const char *arg) {
  char char1[TCP_BUFFER_LENGTH], char2[TCP_BUFFER_LENGTH],
       char3[TCP_BUFFER_LENGTH], char6[TCP_BUFFER_LENGTH];
  time_t tm, tm2;

  char6[0] = '\0';

  switch (cmd) {
    case RPL_WHOISUSER:
      if (sscanf(arg, "%s %s %s", char1, char2, char3) == 3) {
        char *s = strchr(arg, ':');
        if (s) ++s;
        sprintf(char6, "%s is %s@%s", char1, char2, char3);
        if (s && *s) sprintf(char6, "%s (%s)", char6, s);
      }
      break;

    case RPL_WHOWASUSER:
      if (sscanf(arg, "%s %s %s", char1, char2, char3) == 3) {
        char *s = strchr(arg, ':');
        if (s) ++s;
        sprintf(char6, "%s was %s@%s", char1, char2, char3);
        if (s && *s) sprintf(char6, "%s (%s)", char6, s);
      }
      break;

    case RPL_WHOISSERVER:
      if (sscanf(arg, "%s %s", char1, char2) == 2) {
        char *s = strchr(arg, ':');
        if (s) ++s;
        sprintf(char6, "%s is on irc via server %s", char1, char2);
        if (s && *s) sprintf(char6, "%s (%s)", char6, s);
      }
      break;

    case RPL_WHOISCHANNELS:
      if (sscanf(arg, "%s :", char1) == 1) {
        char *s = strchr(arg, ':');
        if (s) ++s;
        sprintf(char6, "%s is on channels: %s", char1, s);
      }
      break;

    case RPL_WHOISIDLE:
      if (sscanf(arg, "%s %s %s", char1, char2, char3) == 3) {
        tm = strtoul(char2, (char **)0, 10);
        printf("tm is %lu\n", tm);
        char *itime = BuildSecondTime(tm);
        tm2 = strtoul(char3, (char **)0, 10);
        printf("tm2 is %lu\n", tm2);
        sprintf(char6, "%s has been idle %s and signon was at %s", 
                        char1, itime, ctime(&tm2));
      }
      break;

    case RPL_ENDOFWHOIS:
      if (sscanf(arg, "%s :", char1) == 1) {
        sprintf(char6, "End of Whois");
      }
      break;

    case RPL_ENDOFWHOWAS:
      if (sscanf(arg, "%s :", char1) == 1) {
        sprintf(char6, "End of Whowas");
      }
      break;

    case RPL_WHOISOPERATOR:
    case RPL_WHOISHELPOP:
    case RPL_WHOISSADMIN:
    case RPL_WHOISADMIN:
    case RPL_WHOISNETWORK:
    case 379:
      if (sscanf(arg, "%s :", char1) == 1) {
        char *s = strchr(arg, ':');
        if (s) ++s;
        sprintf(char6, "%s", char1);
        if (s && *s) sprintf(char6, "%s %s", char6, s);
      }
      break;

    case 378: // Real Hostname
        char *s = strchr(arg, ':');
        if (s) ++s;
        if (s && *s) sprintf(char6, "Real Hostname is%s", s);
      break;

    }

  if (foxircSettings->SendToInfo(P_LOG_WHOIS)) {
    /*GetChannel(char1)->*/Log(char6, 5);
  } else {
  }
}

void OXIrc::CTCPSend(char *nick, char *command, int mode) {
  char s[TCP_BUFFER_LENGTH];

  if ((mode >= 0) && (mode <=1)) {
    sprintf(s, "%s %s :%c%s%c\n", ((mode == NOTICE) ? "NOTICE" : "PRIVMSG"),
               nick, 1, command, 1);
    OTcpMessage message(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, s);
    SendMessage(_irc, &message);
  }
}

unsigned long OXIrc::ModeBits(char *mode_str) {
  int i, bit;
  unsigned long mode_bits = 0L;
    
  if (!mode_str) return 0L;
    
  for (i=0; i<strlen(mode_str); ++i) {
    if (mode_str[i] == ' ') break;
    bit = mode_str[i] - 'a';  // use tolower here?
    if (bit >= 0 && bit <= 26) mode_bits |= (1 << bit);
  }

  return mode_bits;
}

void OXIrc::DoConnect() {
  int retc;

  OString *msg = new OString("");
  OString *title = new OString("Connect to Server");
  OServerInfo *info = new OServerInfo();

  if (*_server) {
    info->hostname = new char[strlen(_server)+1];
    strcpy(info->hostname, _server);
  }

  if (*_nick) {
    info->nick = new char[strlen(_nick)+1];
    strcpy(info->nick, _nick);
  }

  if (*_ircname) {
    info->ircname = new char[strlen(_ircname)+1];
    strcpy(info->ircname, _ircname);
  }

  info->port = _port;

  new OXServerDlg(_client->GetRoot(), this, title, info, &retc);
  if (retc == ID_OK) {
    strcpy(_nick, info->nick);
    strcpy(_ircname, info->ircname);
    Connect(info->hostname, info->port);
  }

  delete info;
}

void OXIrc::DoOpenLog() {
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
    _plog->EnableEntry(802);  // flush
    _plog->EnableEntry(804);  // close
    _plog->EnableEntry(805);  // empty
  }
}

void OXIrc::DoCloseLog() {
  if (_logfile) {
    time_t now = time(NULL);
    fprintf(_logfile, "****** IRC log ended %s", ctime(&now));
    fclose(_logfile);
    _logfile = NULL;
    delete[] _logfile;
    _logfile = NULL;
    _plog->DisableEntry(802);  // flush
    _plog->DisableEntry(804);  // close
    _plog->DisableEntry(805);  // empty
  }
}

void OXIrc::DoFlushLog() {
  if (_logfile) {
    fflush(_logfile);
    _plog->DisableEntry(802);  // flush
  }
}

void OXIrc::DoEmptyLog() {
}

void OXIrc::DoPrintLog() {
}

void OXIrc::DoToggleToolBar() {
  if (_toolBar->IsVisible()) {
    HideFrame(_toolBar);
    HideFrame(_toolBarSep);
    _mview->UnCheckEntry(3001);
  } else {
    ShowFrame(_toolBar);
    ShowFrame(_toolBarSep);
    _mview->CheckEntry(3001);
  }
}

void OXIrc::DoToggleStatusBar() {
  if (_statusBar->IsVisible()) {
    HideFrame(_statusBar);
    _mview->UnCheckEntry(3002);
  } else {
    ShowFrame(_statusBar);
    _mview->CheckEntry(3002);
  }
}

void OXIrc::DoHelpAbout() {
  OAboutInfo info;
  
  info.wname = "About fOXIrc";
  info.title = FOXIRCVERSION"\nA cool IRC client program";
  info.copyright = "Copyright © 1998-1999 by the fOX Project Team.";
  info.text = "This program is free software; you can redistribute it "
              "and/or modify it under the terms of the GNU "
              "General Public License.\n Coders: \n "
	      "Ben Ciaccio, Mike McDonald, Hector Peraza, Rodolphe Suescun\n"
              "irc.foxchat.net  #foxchat";
  info.title_font = (OXFont *) _client->GetFont("Times -14 bold");

  new OXAboutDialog(_client->GetRoot(), this, &info);
}



#define DAYSECS 86400
#define HOURSECS 3600
#define MINUTESECS 60

char *OXIrc::BuildSecondTime(time_t tm) {
  static char char1[TCP_BUFFER_LENGTH];
  unsigned long tem = tm;

  char1[0] = 0;

  // days ?
  if (tem / DAYSECS) {
    sprintf(char1, "%d Days ", tem/DAYSECS);
    tem %= DAYSECS;
  }

  // hours ?
  if (tem / HOURSECS) {
    sprintf(char1, "%s%d Hours ", char1, tem/HOURSECS);
    tem %= HOURSECS;
  }

  // minutes ?
  if (tem / MINUTESECS) {
    sprintf(char1, "%s%d Minutes ", char1, tem/MINUTESECS);
    tem %= MINUTESECS;
  }

  if (tem > 0)
    sprintf(char1, "%s%d Seconds", char1, tem);

  return char1;
}

void OXIrc::DoMotd() {
  SendRawCommand("MOTD");
}

void OXIrc::DoNick() {
  OString *tmp = new OString("");
  int retn;

  new OXConfirmDlg(_client->GetRoot(), this, new OString("Change Nick"),
                   new OString("Please enter Nick"), tmp, &retn);

  if (retn != ID_NO) ChangeNick(tmp->GetString());
  delete tmp;
}

void OXIrc::DoRaw() {
  if (_connected) {
    OString *temp = new OString("");
    int ret;

    new OXConfirmDlg(_client->GetRoot(), this, new OString("Send Raw Command"),
	new OString("Enter raw command to send to server"), temp, &ret);

    if (ret != ID_NO) SendRawCommand(temp->GetString());
    delete temp;
  }
}

void OXIrc::DoWallops() {
  OString *tmp = new OString("");
  int retn;

  new OXConfirmDlg(_client->GetRoot(), this, new OString("Send Wallops"), 
      new OString("Please enter Wallops Message"), tmp, &retn);

  if (retn != ID_NO) SendWallops(tmp->GetString());
  delete tmp;
}

void OXIrc::SendWallops(const char *mess) {
  char m[TCP_BUFFER_LENGTH];

  sprintf(m, "WALLOPS :%s\n", mess);
  SendRawCommand(m);
}

void OXIrc::SendUMode(const char *mod) {
  char m[TCP_BUFFER_LENGTH];

  sprintf(m, "MODE %s %s\n", _nick, mod);
  SendRawCommand(m);
}

void OXIrc::SendMode(const char *chan, const char *mod) {
  char m[TCP_BUFFER_LENGTH];

  sprintf(m, "MODE %s %s\n", chan, mod);
  SendRawCommand(m);
}

void OXIrc::SendRawCommand(const char *command) {
  if(_connected) {
    char m[TCP_BUFFER_LENGTH];

    sprintf(m, "%s\n", command);
    OTcpMessage msg(OUTGOING_TCP_MSG, 0, 0, 0, 0, 0, m);
    SendMessage(_irc, &msg);
    //sprintf(m, "Sent raw command %s.", command);
    //Log(m);
  }
}

void OXIrc::ProcessLink(char *string) {
  char char1[TCP_BUFFER_LENGTH], char2[TCP_BUFFER_LENGTH];
  int  int1;

  if (sscanf(string, "%s %s :%d ", char1, char2, &int1) == 3) { //link
    Linkstrut *link = new Linkstrut();

    link->servername = StrDup(char1);
    link->connectedto = StrDup(char2);
    link->hop = int1;

    char *s = strchr(string, ':');
    if (s) ++s;
    while ((s) && isdigit(s[0])) s++;

    if (s)
      link->servermsg = StrDup(s);
    else
      link->servermsg = NULL;

    Links.Add(link);

  } else {
    sprintf(char1, "Invalid Link %s", string);
    Log(char1, 5);
  }
}
