#ifndef __OXCHATCHANNEL_H
#define __OXCHATCHANNEL_H

#include <stdlib.h>
#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXTransientFrame.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXSlider.h>
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

  int  ProcessCommand(char *cmd);
  void SetChannelMode(char *mode_str);
  void SetChannelMode(unsigned long mode_bits);
  void SetUserMode(unsigned long mode_bits);
  unsigned long GetChannelMode() const { return _cmode; }
  unsigned long GetUserMode() const { return _umode; }
  void EnableChannelMode(char *mode_bits);
  void EnableChannelMode(unsigned long mode_bits);
  void EnableUserMode(unsigned long mode_bits);

  virtual void Log(char *message);
  virtual void Log(char *message, char *color);
  virtual void Log(char *message, int color);

  void DoOpenLog();
  void DoCloseLog();
  void DoFlushLog();
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
  OXPopupMenu *_menuchannel, *_menumode, *_menulog,
              *_menuedit,    *_menuview, *_menuhelp;

  unsigned long _cmode, _ecmode, _umode, _eumode;
  OXStatusBar *_statusBar;

  char *_logfilename;
  FILE *_logfile;
};


#endif  // _OXCHATCHANNEL_H
