/**************************************************************************

    This file is part of foxirc, a cool irc client.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#ifndef __OXPREFERENCES_H
#define __OXPREFERENCES_H


#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXTransientFrame.h>
#include <xclass/OXTab.h>
#include <xclass/OXButton.h>
#include <xclass/OXListBox.h>
#include <xclass/OString.h>
#include <xclass/OPicture.h>

#include "OXSDList.h"
#include "TDList.h"

//---------------------------------------------------------------------

// This class contains all the user-changeable settings in fOXIrc
// Perhaps most of this settings should be implemented as member
// variables (properties) of the corresponding classes instead,
// but keeping them here makes it easier to load and save them.

#define P_CONFIRM_QUIT        (1<<0)
#define P_CONFIRM_LEAVE       (1<<1)
#define P_CONFIRM_SAVE        (1<<2)
#define P_CONFIRM_SQUIT       (1<<3)
#define P_CONFIRM_KILL        (1<<4)

#define P_LOG_CTCP            (1<<0)
#define P_LOG_SIGNOFF         (1<<1)
#define P_LOG_WHO             (1<<2)
#define P_LOG_WHOIS           (1<<3)
#define P_LOG_WHOWAS          (1<<4)
#define P_LOG_ERROR           (1<<5)
#define P_LOG_ISON            (1<<6)
#define P_LOG_INFO            (1<<7)
#define P_LOG_KILL            (1<<8)
#define P_LOG_CLOSE           (1<<9)

#define P_MISC_ENABLE_CMD     (1<<0)
#define P_MISC_NOTIFY_ALL     (1<<1)
#define P_MISC_AUTO_JOIN      (1<<2)
#define P_MISC_SHOW_OPS       (1<<3)
#define P_MISC_SHOW_FRIENDS   (1<<4)
#define P_MISC_SHOW_VOICED    (1<<5)
#define P_MISC_MIRC_COLORS    (1<<6)
#define P_MISC_ANSI_COLORS    (1<<7)
#define P_MISC_POPUP_WINDOW   (1<<8)
#define P_MISC_POPUP_SERV_CN  (1<<9)
#define P_MISC_POPUP_CHAN_CN  (1<<10)
#define P_MISC_TRANSIENT_CHW  (1<<11)

#define P_CHAN_AUTO_RAISE     (1<<0)
#define P_CHAN_AUTO_LOG       (1<<1)


class OServerInfo {
public:
  OServerInfo() : name(0), hostname(0), passwd(0), ircname(0),
                  nick(0), opnick(0), oppasswd(0), 
                  logfile(0), port(6667) {};
  ~OServerInfo();

public:
  char *name, *hostname, *passwd, *ircname, *nick, *opnick, *oppasswd;
  char *logfile;
  int  port;
};

class ONameInfo {
  char *name;
};

//typedef TDDLList<ONameInfo *> ONamePtrList;
//typedef TIDLList<ONameInfo> ONameList;

#define TRANSIENT_WINDOW 	(1<<0)
#define USES_BACKGROUND_PIX 	(1<<1)
#define AUTO_RAISE_WINDOW	(1<<2) // raise,flash,beep are
#define AUTO_FLASH_WINDOW	(1<<3) // for auto notice in channels
#define AUTO_BEEP_WINDOW	(1<<4)
#define AUTO_LOG		(1<<5)

class OChannelInfo {
public:
  OChannelInfo() : name(0), logfile(0),background(0), flags(0) {};
  ~OChannelInfo() { if (name) delete[] name;
                    if (logfile) delete[] logfile;
		    if (background) delete[] background; }

  char *name;
  char *logfile;
  char *background;
  int   flags;
};

//typedef TDDLList<OChannelInfo *> OChannelPtrList;
//typedef TIDLList<OChannelInfo> OChannelList;

class OSettings : public OBaseObject {
public:
  OSettings();
  ~OSettings();

  int Load(char *fname);
  int Save();

//  void AddServer(char *server, int port, char *psw, char *oppsw,
//                 char *defnick, char *opnick);
//  void RemoveServer(char *server);
//  void AddNick(char *nick);
//  void RemoveNick(char *nick);
//  void AddName(char *name);
//  void RemoveName(char *name);

  OServerInfo *FindServer(const char *name);

  OChannelInfo *FindChannel(const char *name);
  int CheckChannelFlags(const char *name, int what);

  OXSDList   *GetServerList()  { return _servers; }
  OXSDList   *GetChannelList() { return _channels; }
  OXSDList   *GetNamesList()   { return _names; }
  OXSDList   *GetNickList()    { return _nicks; }
  OXSDList   *GetColorsList()  { return _colors; }
  const char *GetFont()        { return _font; }
// returns a index into the IRCcolors[] array
// given a color preference index
  int GetColorID(int id);
  int SetColor(int id,int color);


  int Confirm(int what) const { return (_confirm & what); }
  int SendToInfo(int what) const { return (_sendToInfo & what); }
  int CheckMisc(int what) const { return (_misc & what); }

  int Changed() const { return _changed; }

  friend class OXPreferencesDialog;
  friend class OXIrcTab;
  friend class OXNamesTab;
  friend class OXServersTab;
  friend class OXChannelTab;

protected:
  OXSDList *_servers;
  OXSDList *_nicks;
  OXSDList *_names;
  OXSDList *_channels;
  OXSDList *_colors;

  char *_font;
  int _confirm;
  int _sendToInfo;
  int _misc;
  int _changed;

private:
  char *_filename;
};


//---------------------------------------------------------------------

class OXPreferencesDialog : public OXTransientFrame {
public:
  OXPreferencesDialog(const OXWindow *p, const OXWindow *main,
                      OSettings *settings,
                      unsigned long options = MAIN_FRAME | VERTICAL_FRAME);
  virtual ~OXPreferencesDialog();

  virtual int ProcessMessage(OMessage *msg);

protected:
  void UpdateListBox();

  //=== dialog widgets and frames:

  //--- "OK Cancel Apply" buttons:
  OXButton *Ok, *Cancel, *Apply;
  OXHorizontalFrame *bframe;
  OLayoutHints *bly, *bfly;

  //--- Tab widget:
  OXTab *tab;
  OLayoutHints *Ltab;

  OSettings *_settings;
};


//---------------------------------------------------------------------

struct _ButtonDef {
  char *name;
  int  id, mask;
  OXButton *button;
};

class OXIrcTab : public OXHorizontalFrame {
public:
  OXIrcTab(const OXWindow *p, OSettings *settings);
  virtual ~OXIrcTab();

  OXFrame *InitButtons(char *gname, int ctlvar, struct _ButtonDef b[]);
  virtual int ProcessMessage(OMessage *msg);

protected:
  OLayoutHints *_lcb, *_lgf;
  OSettings *_settings;
};


//---------------------------------------------------------------------

class OXNamesTab : public OXHorizontalFrame {
public:
  OXNamesTab(const OXWindow *p, OSettings *settings);
  virtual ~OXNamesTab();

  virtual int ProcessMessage(OMessage *msg);

protected:
  OLayoutHints *_lgf, *_lbt, *_llb, *_lvf;
  OSettings *_settings;
};


//---------------------------------------------------------------------

class OXServersTab : public OXHorizontalFrame {
public:
  OXServersTab(const OXWindow *p, OSettings *settings);
  virtual ~OXServersTab();

  virtual int ProcessMessage(OMessage *msg);

protected:
  void _doAdd();
  void _doEdit();
  void _ReadList();

  OXButton *_add, *_edit, *_delete, *_up, *_down;
  OXListBox *lb;
  OLayoutHints *_lbt, *_llb, *_lvf;
  OSettings *_settings;
};

//---------------------------------------------------------------------

struct OColorsPref {
  char *name;
  long defColor;
  long id;
};

class OXColorsTab : public OXHorizontalFrame {
public:
  OXColorsTab(const OXWindow *p, OSettings *settings);
//  virtual ~OXNamesTab();

  virtual int ProcessMessage(OMessage *msg);

protected:
//  OLayoutHints *_lgf, *_lbt, *_llb, *_lvf;
  OSettings *_settings;
};

//---------------------------------------------------------------------

class OXChannelTab : public OXHorizontalFrame {
public:
  OXChannelTab(const OXWindow *p, OSettings *settings);
  virtual ~OXChannelTab();

  virtual int ProcessMessage(OMessage *msg);

protected:
  void _DoAdd();
  void _DoEdit();
  void _ReadList();

  OXButton *_add, *_edit, *_delete; //, *_up, *_down;
  OXListBox *lb;
  OLayoutHints *_lbt, *_llb, *_lvf;
  OSettings *_settings;
};

#endif   // __OXPREFERENCES_H
