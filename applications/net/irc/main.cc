#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

#include <xclass/OXMenu.h>

#include "OXIrc.h"
#include "OXChannel.h"
#include "OXNameList.h"
#include "OXPreferences.h"

OXClient *clientX;
OXIrc *mainWindow;
OXPopupMenu *_nick_actions, *_nick_ctcp, *_nick_dcc, *_nick_ignore;

OSettings  *foxircSettings;// = new OSettings();

int main(int argc, char **argv)
{
  char server[50], nick[15];
  char foxircrc[PATH_MAX];
  int  port = -1;

  clientX = new OXClient;
  foxircSettings = new OSettings();

  _nick_ctcp = new OXPopupMenu(clientX->GetRoot());
  _nick_ctcp->AddEntry(new OHotString("Clientinfo"), CTCP_CLIENTINFO);
  _nick_ctcp->AddEntry(new OHotString("Finger"),     CTCP_FINGER);
  _nick_ctcp->AddEntry(new OHotString("Ping"),       CTCP_PING);
  _nick_ctcp->AddEntry(new OHotString("Time"),       CTCP_TIME);
  _nick_ctcp->AddEntry(new OHotString("Version"),    CTCP_VERSION);
  _nick_ctcp->AddEntry(new OHotString("Userinfo"),   CTCP_USERINFO);
  _nick_ctcp->AddEntry(new OHotString("Other..."),   CTCP_OTHER);

  _nick_dcc = new OXPopupMenu(clientX->GetRoot());
  _nick_dcc->AddEntry(new OHotString("Send"), DCC_SEND);
  _nick_dcc->AddEntry(new OHotString("Chat"), DCC_CHAT);

  _nick_ignore = new OXPopupMenu(clientX->GetRoot());
  _nick_ignore->AddEntry(new OHotString("All"),     0);
  _nick_ignore->AddSeparator();
  _nick_ignore->AddEntry(new OHotString("Clear"),   0);
  _nick_ignore->AddEntry(new OHotString("Msgs"),    0);
  _nick_ignore->AddEntry(new OHotString("Notices"), 0);
  _nick_ignore->AddEntry(new OHotString("Public"),  0);
  _nick_ignore->AddEntry(new OHotString("Invites"), 0);
  _nick_ignore->AddEntry(new OHotString("Wallops"), 0);
  _nick_ignore->AddEntry(new OHotString("Notes"),   0);
  _nick_ignore->AddEntry(new OHotString("CTCP"),    0);
  _nick_ignore->AddEntry(new OHotString("Others"),  0);

  _nick_actions = new OXPopupMenu(clientX->GetRoot());
  _nick_actions->AddEntry(new OHotString("Whois"),    C_WHOIS);
  _nick_actions->AddEntry(new OHotString("Message"),  C_MESSAGE);
  _nick_actions->AddEntry(new OHotString("Notice"),   C_NOTICE);
  _nick_actions->AddEntry(new OHotString("Ping"),     C_PING);
  _nick_actions->AddEntry(new OHotString("Time"),     C_TIME);
  _nick_actions->AddPopup(new OHotString("CTCP"),     _nick_ctcp);
  _nick_actions->AddPopup(new OHotString("DCC"),      _nick_dcc);
  _nick_actions->AddEntry(new OHotString("Notify"),   C_NOTIFY);
  _nick_actions->AddPopup(new OHotString("Ignore"),   _nick_ignore);
  _nick_actions->AddEntry(new OHotString("Finger"),   C_FINGER);
  _nick_actions->AddEntry(new OHotString("Speak"),    C_SPEAK);
  _nick_actions->AddEntry(new OHotString("ChanOp"),   C_CHAN_OP);
  _nick_actions->AddEntry(new OHotString("Kick"),     C_KICK);
  _nick_actions->AddEntry(new OHotString("Ban"),      C_BAN);
  _nick_actions->AddEntry(new OHotString("Ban+Kick"), C_BANKICK);
  _nick_actions->AddEntry(new OHotString("Kill"),     C_KILL);

  _nick_actions->DisableEntry(C_SPEAK);
  _nick_actions->DisableEntry(C_CHAN_OP);
  _nick_actions->DisableEntry(C_KILL);

  sprintf(foxircrc, "%s/.foxircrc", getenv("HOME") ? getenv("HOME") : "/");
  foxircSettings->Load(foxircrc);

  nick[0] = 0;
  server[0] = 0;

  if (getenv("IRCSERVER")) strncpy(server, getenv("IRCSERVER"), 14);
  if (getenv("IRCNICK")) strncpy(nick, getenv("IRCNICK"), 49);

  if (argc > 1) strncpy(nick, argv[1], 14);
  if (argc > 2) strncpy(server, argv[2], 49); 
  if (argc > 3) port = atoi(argv[3]);

  if (port < 0) port = 6667;

  nick[14] = 0;
  server[49] = 0;

//  if ((nick[0] == 0) || (server[0] == 0)){
//    printf("Usage: %s nick server [port]\n", argv[0]);
//    exit(1);
//  }

  // Ignore SIGPIPE errors so that the application don't die when the
  // remote end closes the connection. This will cause write (send)
  // operations on sockets to return with errno = EPIPE in these cases.

  signal(SIGPIPE, SIG_IGN);

  mainWindow = new OXIrc(clientX->GetRoot(), nick, server, port);
  mainWindow->MapWindow();

  clientX->Run();

  return 0;
}
