#ifndef __OXCHATCHANNEL_H
#define __OXCHATCHANNEL_H

#include <stdlib.h>
#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXTransientFrame.h>
#include <xclass/OXMenu.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OX3dLines.h>
#include <xclass/OString.h>

#include "OXChannel.h"


//----------------------------------------------------------------------

class OXChatChannel : public OXChannel {
public:
  OXChatChannel(const OXWindow *p, const OXWindow *main, OXIrc *s,
                const char *ch);
  virtual ~OXChatChannel();

  virtual int CloseWindow();
  virtual int ProcessMessage(OMessage *msg);

  virtual int ProcessCommand(char *cmd);

  void SetChannelMode(const char *mode_str);
  void SetChannelMode(unsigned long mode_bits);
  void SetUserMode(unsigned long mode_bits);
  void UpdateUserMode();
  unsigned long GetChannelMode() const { return _cmode; }
  unsigned long GetUserMode() const { return _umode; }
  void EnableChannelMode(const char *mode_bits);
  void EnableChannelMode(unsigned long mode_bits);
  void EnableUserMode(unsigned long mode_bits);

  virtual void Log(const char *message);
  virtual void Log(const char *message, int color);

  void DoOpenLog();
  void DoCloseLog();
  void DoEmptyLog();
  void DoPrintLog();
  void DoToggleToolBar();
  void DoToggleStatusBar();
  void DoToggleTopicBar();
  int  DoLeave();

protected:
  OXCanvas *_canvas;
  OXNameList *_nlist;
  void DoChannelMode(const char *mode);

  OXHorizontal3dLine *_topicsep;
  OXCompositeFrame *_ftopic;
  OXLabel *_ltopic;
  OXTextEntry *_topic;

  OXMenuBar *_menubar;
  OXPopupMenu *_menuchannel, *_menumode, *_menulog, *_menuedit,
              *_menuview, *_menuhelp, *_nick_actions, *_nick_ctcp,
              *_nick_dcc, *_nick_ignore;

  unsigned long _cmode, _ecmode, _umode, _eumode;
  int _chlimit;
  OXStatusBar *_statusBar;

  char *_logfilename;
  FILE *_logfile;
};


#endif  // _OXCHATCHANNEL_H
