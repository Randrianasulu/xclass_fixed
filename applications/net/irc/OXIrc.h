#ifndef __OXIRC_H
#define __OXIRC_H

#define FOXIRCVERSION "fOXIrc v0.02-990614 http://www.foxproject.org/"

#include <stdlib.h>
#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OXToolBar.h>
#include <xclass/OXStatusBar.h>
#include <xclass/OXMenu.h>
#include <xclass/OXButton.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXSlider.h>
#include <xclass/OX3dLines.h>
#include <xclass/OString.h>
#include <xclass/OMimeTypes.h>
#include <xclass/OXFileDialog.h>
#include <xclass/OXAboutDialog.h>
#include <xclass/OFileHandler.h>

#include "OIrc.h"
#include "OXTextView.h"
#include "OXPreferences.h"
#include "TDList.h"


#define MENU_DISABLED     (1<<0)
#define MENU_CHECKED      (1<<1)
#define MENU_RCHECKED     (1<<2)

struct SPopupData {
  OXPopupMenu *ptr;
  struct {
    char *name;
    int  id, state;
    SPopupData *popup_ref;
  } popup[20];
};

//---------------------------------------------------------------------

class OXChannel;

struct Linkstrut {
  char *servername;
  char *connectedto;
  char *servermsg;
  int hop;
};

typedef TDDLList<Linkstrut *> LinkList;


class OXIrc : public OXMainFrame {
public:
  OXIrc(const OXWindow *p, char *nick = NULL,
        char *server = NULL, int port = 6667);
  virtual ~OXIrc();

  virtual void CloseWindow();
  virtual int  ProcessMessage(OMessage *msg);
  virtual int  HandleMapNotify(XMapEvent *event);
  virtual int  HandleFileEvent(OFileHandler *fh, unsigned int mask);

  int Connect(char *server, int port = 6667);
  int Disconnect();

  void Log(const char *message);
  void Log(const char *message, char *color);
  void Log(const char *message, int color);

  OXChannel *GetChannel(const char *channel);
  OXChannel *FindChannel(const char *channel);
  void RemoveChannel(OXChannel *channel);

  void ProcessWhois(int cmd, const char *arg);
  void CTCPSend(char *nick, char *command, int mode);
  void SendRawCommand(const char *command);
  void ChangeNick(const char *nick);
  void SendWallops(const char *mess);
  void SendUMode(const char *mod);
  void SendMode(const char *chan,const char *mod);
  void ProcessLink(char *string);
  int  Connected() const { return _connected; }

  unsigned long ModeBits(char *mode_str);

  OXPopupMenu *MakePopup(SPopupData *p);

  void DoConnect();
  void DoOpenLog();
  void DoCloseLog();
  void DoFlushLog();
  void DoEmptyLog();
  void DoPrintLog();
  void DoToggleToolBar();
  void DoToggleStatusBar();
  void DoHelpAbout();
  void DoRaw();
  void DoNick();
  void DoWallops();
  void DoMotd();

  char _nick[15];
  char _ircname[256];
  char _username[100];
  char _hostname[100];
  OIrc *GetOIrc() { return _irc; }

protected:
  char *BuildSecondTime(time_t tm);

  OIrc *_irc;

  OFileHandler *_fh;
  OTimer *_timer;

  OXTextEntry *_channelentry;

  OTextDoc *_log;
  OXViewDoc *_logw;

  char _server[80];
  int _init, _port, _fd, _connected;

  OXChannel *channels;

  OXMenuBar *_menubar;
  OXPopupMenu *_mirc, *_medit, *_mview, *_mhelp;
  OXPopupMenu *_pserv, *_pusers, *_pchan, *_plog;

  OXToolBar *_toolBar;
  OXHorizontal3dLine *_toolBarSep;
  OXStatusBar *_statusBar;

  unsigned long _avcmode, _avumode;
  
  char *_logfilename;
  FILE *_logfile;
  LinkList Links;
};

#endif  // __OXIRC_H
