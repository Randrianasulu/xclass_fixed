#ifndef __OXNAMELIST_H
#define __OXNAMELIST_H

#define NICK_OPED   1<<0
#define NICK_VOICED 1<<1

#define C_PING      1
#define C_WHOIS     2
#define C_MESSAGE   3
#define C_NOTICE    4
#define C_TIME      5
#define C_NOTIFY    6
#define C_FINGER    7
#define C_SPEAK     8
#define C_CHAN_OP   9
#define C_KICK     10
#define C_BAN      11
#define C_BANKICK  12
#define C_KILL     13

#include <xclass/OXMenu.h>

class OXNameList;

class OXName : public OXMenuTitle {
public:
  OXName(const OXWindow *p, OHotString *s, const OPicture *pic, 
         OXPopupMenu *menu, unsigned int options = 0);

  virtual ODimension GetDefaultSize() const 
    { return ODimension(_tw+8, _th+4); }

  void SetState(int state);
  void SetPicture(const OPicture *p);

  void SetFlags(int flags) 	{ _flags |= flags; 		}
  void ClearFlags(int flags) 	{ _flags &= ~flags; 		}
  bool IsOp()			{ return _flags & NICK_OPED;  	}
  bool IsVoiced()		{ return _flags & NICK_VOICED;	}
  
  void DoSendMessage();
  void Associate(const OComponent *o) { _msgObject = o; } 

  friend class OXClient;
  friend class OXNameList;

protected:
  void _DoRedraw();
  void _DrawTrianglePattern(GC gc, int l, int t, int r, int b);

  const OComponent *_msgObject;
  const OPicture *_pic;
  int _tw, _th;
  int _flags;
};

class OXNameList : public OXVerticalFrame {
public:
  OXNameList(const OXWindow *p, OXPopupMenu *popup, int w, int h,
	    unsigned int options = VERTICAL_FRAME);
  ~OXNameList();

  void AddPopup(OHotString *s, const OPicture *p,
                OXPopupMenu *menu, OLayoutHints *l);

  virtual int HandleButton(XButtonEvent *event);
  virtual int HandleMotion(XMotionEvent *event);

  OXName *GetName(const char *s);
  bool HaveNick(const char *s);
  void AddName(const char *s);
  void RemoveName(const char *s);
  void ChangeName(const char *o, const char *n);
  void OpNick(const char *nick);
  void VoiceNick(const char *nick);
  void NormalNick(const char *nick);

  int  GetTotalCount(){return _total;}
  int  GetOpedCount(){return _oped;}
  int  GetVoicedCount(){return _voiced;}

  void Associate(const OComponent *o) { _msgObject = o; } 

protected:
  OXName *_current;
  int _stick; 
  OXPopupMenu *_popup;
  const OComponent *_msgObject;
  const OPicture *_pchanop, *_pvoice;
  int _total,_oped,_voiced;
};


#endif  // __OXNAMELIST_H
