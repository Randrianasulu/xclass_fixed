#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <errno.h>

#include "OXDCCFile.h"
#include "IRCcodes.h"
#include "OIrcMessage.h"
#include "OXMsgChannel.h"
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

OXMsgChannel::OXMsgChannel(const OXWindow *p, const OXWindow *main,
                           OXIrc *s, const char *ch) :
  OXChannel(p, main, s, ch, true) {
    char wname[100];
/*
  int  ax, ay;
  Window wdummy;

  _server = s;
  _next = _prev = NULL;
  strcpy(_name, ch);

  SetLayoutManager(new OVerticalLayout(this));

  _UserLabel = new OXLabel(this,new OHotString("Nick"));
  _ServerLabel = new OXLabel(this,new OHotString("Server"));
  _InfoLabel = new OXLabel(
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


    //---- position relative to the parent's window
 
    XTranslateCoordinates(GetDisplay(),
                          s->GetId(), GetParent()->GetId(),
                          50, 50, &ax, &ay, &wdummy);

    Move(ax, ay);

    MapWindow();

    sprintf(wname, "Conversation with %s", _name);
*/
    sprintf(wname, "%s Private Chat", _name);
    SetWindowName(wname);
}
