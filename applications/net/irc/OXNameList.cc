#include <xclass/OXCompositeFrame.h>
#include <xclass/OXButton.h>
#include <xclass/OXFont.h>
#include <xclass/OGC.h>

#include "OXNameList.h"
#include "OIrcMessage.h"

#include "pixmaps/nl-chanop.xpm"
#include "pixmaps/nl-voice.xpm"


//-----------------------------------------------------------------

extern OLayoutHints *topexpandxlayout0;

OXNameList::OXNameList(const OXWindow *p, OXPopupMenu *popup, int w, int h,
                       unsigned int options) :
  OXVerticalFrame(p, w, h, options | VERTICAL_FRAME) {

    _pchanop = _client->GetPicture("nl-chanop.xpm", nl_chanop);
    _pvoice  = _client->GetPicture("nl-voice.xpm",  nl_voice);

    _popup = popup;
    _stick = True;
    _current = NULL;

    XGrabButton(GetDisplay(), Button1, AnyModifier, _id, True,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask,
		GrabModeAsync, GrabModeAsync, None, None);
 _total = _voiced = _oped = 0;
}

OXNameList::~OXNameList() {
  SListFrameElt *ptr;
  OXName *t;

  for (ptr = _flist; ptr != NULL; ptr = ptr->next) {
    t = (OXName *) ptr->frame;
  }

  _client->FreePicture(_pchanop);
  _client->FreePicture(_pvoice);
}

int OXNameList::HandleMotion(XMotionEvent *event) {
  SListFrameElt *ptr;
  OXName *target;

  _stick = False; // use some threshold!

  target = (OXName*)_client->GetWindowById(event->subwindow);

  if (target != NULL && target != _current) {
  // Deactivate all others :
    for (ptr = _flist; ptr != NULL; ptr = ptr->next)
      ((OXMenuTitle*)ptr->frame)->SetState(False);

    target->SetState(True);
    _stick = True;
    _current = target;
  }

  return True;
}

void OXNameList::OpNick(const char *nick) {
OXName *n = GetName(nick);
if(n){
     n->SetPicture(_pchanop);
     n->SetFlags(NICK_OPED);
     _oped++;
     }
	
}

void OXNameList::VoiceNick(const char *nick) {
OXName *n = GetName(nick);
if(n){
     n->SetPicture(_pvoice);
     n->SetFlags(NICK_VOICED);
     _voiced++;
     }
}
  
void OXNameList::NormalNick(const char *nick) {
OXName *n = GetName(nick);
if(n){
     n->SetPicture(NULL);
     if(n->IsOp())
     	_oped--;
     if(n->IsVoiced())
     	_voiced--;
     n->ClearFlags(NICK_VOICED|NICK_OPED);
     }
}

int OXNameList::HandleButton(XButtonEvent *event) {
  SListFrameElt *ptr;

  // Here we are looking for a _child_ of a menu bar, so it can _ONLY_ be
  // a MenuTitle : BEWARE !

  OXName *target;

  // We don't need to check the button number as XGrabButton will
  // only allow button1 events

  if (event->type == ButtonPress) {

    target = (OXName*)_client->GetWindowById(event->subwindow);

    if (target != NULL) {
      _stick = True;

      if (target != _current) {
        // Deactivate all others :
        for (ptr = _flist; ptr != NULL; ptr = ptr->next)
          ((OXName*)ptr->frame)->SetState(False);

        target->SetState(True);
        _stick = True;
        _current = target;

        XGrabPointer(GetDisplay(), _id, True,
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                     GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
      }
    }
  }

  if (event->type == ButtonRelease) {
    if (_stick) {
      _stick = False;
      return True;
    }
    XUngrabPointer(GetDisplay(), CurrentTime);

    for (ptr = _flist; ptr != NULL; ptr = ptr->next)
      ((OXName*)ptr->frame)->SetState(False);

    if (_current != NULL) {
      target = _current; /* tricky, because WaitFor */
      _current = NULL;
      target->DoSendMessage();
    }
  }

  return True;
}

void OXNameList::AddPopup(OHotString *s, const OPicture *p,
                          OXPopupMenu *menu, OLayoutHints *l) {
  OXName *name;

  AddFrame(name = new OXName(this, s, p, menu), l);
  name->Associate(_msgObject);
}

OXName *OXNameList::GetName(const char *s) {
  SListFrameElt *ptr;
  OXName *t;

  for (ptr = _flist; ptr != NULL; ptr = ptr->next) {
    t = (OXName *) ptr->frame;
    if (!strcasecmp(s, t->_label->GetString()))
      return t;
  }
  return NULL;
}

bool OXNameList::HaveNick(const char *s) {
  SListFrameElt *ptr;
  OXName *t;

  for (ptr = _flist; ptr != NULL; ptr = ptr->next) {
    t = (OXName *) ptr->frame;
    if (!strcasecmp(s, t->_label->GetString()))
      return true;
  }
  return false;
}

void OXNameList::AddName(const char *s) {
  const OPicture *pic = NULL;
  const char *nick = s;
  OXName *name;

  if (*nick == '@') { pic = _pchanop; ++nick; }
  else if (*nick == '+') { pic = _pvoice; ++nick; }

  name = GetName(nick);
  if (!name) {
    AddPopup(new OHotString(nick), pic, _popup, topexpandxlayout0);
    MapSubwindows();
    Layout();
    _total++;
    name = GetName(nick);
    if(pic==_pchanop)
    	{
    	 name->SetFlags(NICK_OPED);
	 _oped++;
	 }
    if(pic==_pvoice)
    	{
    	name->SetFlags(NICK_VOICED);
	_voiced++;
	}

  } else {
    name->SetPicture(pic);
  }
}

void OXNameList::RemoveName(const char *s) {
  OXName *t = GetName(s);

  if (t) {
     if(t->IsOp())
     	_oped--;
     if(t->IsVoiced())
     	_voiced--;
    _total--;
    RemoveFrame(t);
    XClearWindow(_client->GetDisplay(), t->_id); // ???
    delete t;
    MapSubwindows();
    Layout();
  }
}

void OXNameList::ChangeName(const char *o, const char *n) {
  const char *nick = n;
  const OPicture *pic = NULL;
  OXName *t = GetName(o);

// assume that the modes are the same

//  if (*nick == '@') { ++nick; }
//  else if (*nick == '+') { ++nick; }

  if (t) {
    delete (t->_label);
    t->_label = new OHotString(nick);
//    t->SetPicture(pic);
    t->_DoRedraw();
  }
}

//-----------------------------------------------------------------

OXName::OXName(const OXWindow *p, OHotString *s, const OPicture *pic,
               OXPopupMenu *menu, unsigned int options) :
  OXMenuTitle(p, s, menu, options) {

  _tw = _font->TextWidth(_label->GetString(), _label->GetLength());
  _th = _font->TextHeight();

  _pic = pic;
  _flags = 0;
} 

void OXName::SetState(int state) {
  _state = state;
  if (state) {
    if (_menu != NULL) {
      int ax, ay;
      Window wdummy;

      XTranslateCoordinates(GetDisplay(), _id, (_menu->GetParent())->GetId(),
			0, 0, &ax, &ay, &wdummy);

      // Place the menu just under the window :
      _menu->PlaceMenu(ax+_w, ay+2, True, False); //True);
    }
  } else {
    if (_menu != NULL) {
      _ID = _menu->EndMenu();
    }
  }
  _client->NeedRedraw(this);
}

void OXName::_DoRedraw() {
  const char *name = _label->GetString();
  int x, y, xp;
  OFontMetrics fm;

  _font->GetFontMetrics(&fm);

  x = (_w - _font->TextWidth(name, strlen(name))) >> 1;
  xp = 3 + _bw;
  y = 2;
  if (_state) {
    _options |= SUNKEN_FRAME;
    _options &= ~RAISED_FRAME;
    ++x; ++y; ++xp;
  } else {
    _options |= RAISED_FRAME;
    _options &= ~SUNKEN_FRAME;
  }
  OXFrame::_DoRedraw();
  if (_state) _DrawTrianglePattern(_normGC->GetGC(), _w-10, y+3, _w-6, y+11);
  _label->Draw(GetDisplay(), _id, _normGC->GetGC(), x, y + fm.ascent);
  if (_pic) _pic->Draw(GetDisplay(), _id, _normGC->GetGC(), xp, y+1);
}

void OXName::SetPicture(const OPicture *p) {
  _pic = p;
  _client->NeedRedraw(this);
}

void OXName::_DrawTrianglePattern(GC gc, int l, int t, int r, int b) {
  XPoint points[3];

  int m = (t + b) >> 1;

  points[0].x = l;
  points[0].y = t;
  points[1].x = l;
  points[1].y = b;
  points[2].x = r;
  points[2].y = m;

  XFillPolygon(GetDisplay(), _id, gc, points, 3, Convex, CoordModeOrigin);
}

void OXName::DoSendMessage() {
  if (_ID != -1) {
    OIrcMessage message(IRC_NICKLIST, MSG_CLICK, _ID, 0L, 0L, 0L,
                        (char *)_label->GetString());
    SendMessage(_msgObject, &message);
  }
}
