#include <xclass/OXCompositeFrame.h>
#include <xclass/OXTextButton.h>
#include <xclass/OXIcon.h>

class OXColorFrame : public OXFrame{
public:
 OXColorFrame(const OXWindow *p,unsigned long c,int n );

virtual int HandleButton(XButtonEvent *event);
virtual void DrawBorder();
void RedrawSelf() { NeedRedraw(); }
void Associate(const OComponent *_com){ _msgCom = _com; }
void SetActive(bool in){_active = in;}
protected:
 OXFrame *cf;
 unsigned long color;
 int id;
 bool _active;
 const OComponent *_msgCom;
 GC grayGC;
};

//-----------------------------------------------------------------------------
class OXColorTrigger;
class OX16ColorSelector : public OXCompositeFrame{
public:
      OX16ColorSelector(const OXWindow *p);
virtual int ProcessMessage(OMessage *msg);
void Associate(const OComponent *fr){ _msgwin = fr; }
void SetActive(int newat);
int GetActive() {return active;}
protected:
OXColorFrame *ce[16];
const OComponent *_msgwin;
int active;

};

//------------------------------------------------------------------------------

class OX16ColorDialog: public OXCompositeFrame{
public:
OX16ColorDialog(const OXWindow *p);
virtual int ProcessMessage(OMessage *msg);
void PlacePopup(int x, int y, int w, int h);
void EndPopup();
void Associate(const OComponent *w) { _msgObject = w; }

protected:

const OComponent *_msgObject;
OX16ColorSelector *_cs;
int *rt;
int active;
OXTextButton *_Other;
//OXColorTrigger *test;

};

//-----------------------------------------------------------------------------

class OXColorCell : public OXFrame{
public:
OXColorCell(const OXWindow *p,unsigned long c );
virtual void DrawBorder();
void SetColor(unsigned long _color);
unsigned long GetColor(){return color;}
protected:
unsigned long color;
};

//-----------------------------------------------------------------------------

class OXColorTrigger: public OXCompositeFrame{
public:
OXColorTrigger(const OXWindow *p,unsigned long color,int num);
virtual int HandleButton(XButtonEvent *event);
void Associate(const OComponent *w) { _msgObject = w; }
virtual int ProcessMessage(OMessage *msg);

protected:
OXColorCell *colorCell;
OXIcon *arrow;
const OComponent *_msgObject;
int id;
};

