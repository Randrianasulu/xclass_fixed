#include <stdio.h>
#include "OXColorSelect.h"
#include <xclass/OX3dLines.h>
#include <xclass/OXWindow.h>
#include <xclass/OResourcePool.h>
#include <xclass/OGC.h>
#include <xclass/OXCanvas.h>
#include <X11/cursorfont.h>

#include "small-arrow-down.xpm"
extern unsigned long IRCcolors[];
//------------------------------------------------------------------------------

OXColorFrame::OXColorFrame(const OXWindow *p,unsigned long c,int n ):
 OXFrame(p,20,20,SUNKEN_FRAME|DOUBLE_BORDER|OWN_BKGND,c),
 color(c),id(n){
 
 XGrabButton(GetDisplay(), AnyButton, AnyModifier, _id, False,
		ButtonPressMask | ButtonReleaseMask |
		EnterWindowMask | LeaveWindowMask,
		GrabModeAsync, GrabModeAsync, None, None);
_msgObject = p;
_active= false;
grayGC=_client->GetResourcePool()->GetFrameBckgndGC()->GetGC();
}

int OXColorFrame::HandleButton(XButtonEvent *event) {
    if (event->type == ButtonPress) {
     OContainerMessage msg(MSG_CONTAINER,MSG_CLICK,id,event->button);
     SendMessage(_msgObject, &msg);
     return True;
    }
    if (event->type == ButtonRelease) {
     OContainerMessage msg(MSG_CONTAINER,MSG_SELCHANGED,id,event->button);
     SendMessage(_msgObject, &msg);
     return True;
    }
    return False;
}

void OXColorFrame::DrawBorder() {



if(_active){
		DrawLine(_blackGC,_w-1,0,0,0);
		DrawLine(_blackGC,0,0,0,_h-1);
		DrawLine(_blackGC,0,_h-1,_w-1,_h-1);
		DrawLine(_blackGC,_w-1,_h-1,_w-1,0);

		DrawLine(_whiteGC,_w-2,1,1,1);
		DrawLine(_whiteGC,1,1,1,_h-2);
		DrawLine(_whiteGC,1,_h-2,_w-2,_h-2);
		DrawLine(_whiteGC,_w-2,_h-2,_w-2,1);

		DrawLine(_blackGC,_w-3,2,2,2);
		DrawLine(_blackGC,2,2,2,_h-3);
		DrawLine(_blackGC,2,_h-3,_w-3,_h-3);
		DrawLine(_blackGC,_w-3,_h-3,_w-3,2);
		
}else {
		DrawLine(grayGC,_w-1,0,0,0);
		DrawLine(grayGC,0,0,0,_h-1);
		DrawLine(grayGC,0,_h-1,_w-1,_h-1);
		DrawLine(grayGC,_w-1,_h-1,_w-1,0);
		OXFrame::_Draw3dRectangle(DOUBLE_BORDER|SUNKEN_FRAME,1,1,_w-2,_h-2);
	}


}
//------------------------------------------------------------------------------
OX16ColorSelector::OX16ColorSelector(const OXWindow *p): 
	OXCompositeFrame(p,10,10)
{
 SetLayoutManager(new OMatrixLayout(this, 4, 4,0,0));
// fgpointer = bgpointer = 0;
 for (int i=0;i<16;i++)
 {
  ce[i]=new OXColorFrame(this,IRCcolors[i],i);
  AddFrame(ce[i],new OLayoutHints(LHINTS_CENTER_X|LHINTS_CENTER_Y));
 }
//fg=_client->GetPicture("fg.xpm",fg_xpm);
//bg=_client->GetPicture("bg.xpm",bg_xpm);
//fb=_client->GetPicture("fb.xpm",fb_xpm);
_msgObject = p;
active=-1;
}

void OX16ColorSelector::SetActive(int newat){
if(active!= newat ){
	if((active>=0)&&(active<=15)){
	ce[active]->SetActive(false);
	ce[active]->RedrawSelf();
	}
	active = newat;
	if((active>=0)&&(active<=15)){
		ce[active]->SetActive(true);
		ce[active]->RedrawSelf();
		} else
			printf("Invalid active %d\n",newat);
	}
}
int OX16ColorSelector::ProcessMessage(OMessage *msg){
  OContainerMessage *cmsg = (OContainerMessage *) msg;
//printf("OX16ColorSelector::ProcessMessage(%d,%d,%d,%d)\n",
//	cmsg->type, cmsg->action, cmsg->id, cmsg->button);

switch(msg->type){
	case MSG_CONTAINER:
		switch(msg->action){
			case MSG_SELCHANGED:
				switch(cmsg->button){
					case Button1:
				        OContainerMessage msg1(MSG_CONTAINER,MSG_SELCHANGED,active,0);
				        SendMessage(_msgObject, &msg1);
					break;
					}
			case MSG_CLICK:
				switch(cmsg->button){
					case Button1:
						{
						SetActive(cmsg->id);
						}
						break;
					}
				}
			}
return False;
}
//------------------------------------------------------------------------------
OX16ColorDialog::OX16ColorDialog(const OXWindow *p)
	 :OXCompositeFrame(p,10,10,
 	DOUBLE_BORDER|RAISED_FRAME|OWN_BKGND,_defaultFrameBackground) {

_msgObject=0;
XSetWindowAttributes wattr;
unsigned long mask;

mask = CWOverrideRedirect; // | CWSaveUnder ;
wattr.override_redirect = True;
//wattr.save_under = True;
XChangeWindowAttributes(GetDisplay(), _id, mask, &wattr);

AddInput(StructureNotifyMask);

active = -1;
_cs = new OX16ColorSelector(this);
AddFrame(_cs,new OLayoutHints(LHINTS_CENTER_X,1,1,1,1));
AddFrame(new OXHorizontal3dLine(this),
	 new OLayoutHints(LHINTS_EXPAND_X|LHINTS_CENTER_Y, 3, 3, 2, 2));
_Other = new OXTextButton(this, new OHotString("&Other..."),1002);
_Other->Associate(this);
AddFrame(_Other,new OLayoutHints(LHINTS_EXPAND_X,2,10,2,2));

MapSubwindows();

Resize(_cs->GetDefaultWidth()+6,_cs->GetDefaultHeight()+_Other->GetDefaultHeight());

Cursor defaultCursor = XCreateFontCursor(GetDisplay(), XC_left_ptr);
XDefineCursor(GetDisplay(), GetId(), defaultCursor);

}

void OX16ColorDialog::EndPopup() {
  UnmapWindow();
}

void OX16ColorDialog::PlacePopup(int x, int y, int w, int h) {
  int rx, ry;
  unsigned int rw, rh;
  unsigned int dummy;
  Window wdummy;
  // Parent is root window for the popup:
  XGetGeometry(GetDisplay(), _parent->GetId(), &wdummy,
               &rx, &ry, &rw, &rh, &dummy, &dummy);

  if (x < 0) x = 0;
  if (x + _w > rw) x = rw - _w;
  if (y < 0) y = 0;
  if (y + _h > rh) y = rh - _h;

  MoveResize(x, y, w, h);
  MapSubwindows();
  Layout();
  MapRaised();

//  XGrabPointer(GetDisplay(), _id, True,
//               ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
//               GrabModeAsync, GrabModeAsync, None,
//               GetResourcePool()->GetGrabCursor(), CurrentTime);

  _client->WaitForUnmap(this);
  EndPopup();
}

int OX16ColorDialog::ProcessMessage(OMessage *msg){
  OContainerMessage *cmsg = (OContainerMessage *) msg;

//printf("OX16ColorDialog::ProcessMessage(%d,%d,%d,%d)\n",
//	cmsg->type, cmsg->action, cmsg->id, cmsg->button);

  switch(msg->type){
    case MSG_CONTAINER:
      switch(msg->action){
	case MSG_SELCHANGED:
		{
//		printf("--SendMessage(SEL_CHANGED [%d])\n",cmsg->button);
		OContainerMessage msg1(MSG_CONTAINER,MSG_SELCHANGED,0,cmsg->button);
		SendMessage(_msgObject, &msg1);

		UnmapWindow();
		return true;
		}
	  // cmsg->button is the new color;
	  break;
	default:
	  break;
      }
    case MSG_BUTTON:
      switch (msg->action) {
        case MSG_CLICK:
		break;
		}
    
  }

}

//------------------------------------------------------------------------------
OXColorTrigger::OXColorTrigger(const OXWindow *p,unsigned long color,int num):
	OXCompositeFrame(p,50,21,DOUBLE_BORDER|RAISED_FRAME|HORIZONTAL_FRAME) {

 XGrabButton(GetDisplay(), AnyButton, AnyModifier, _id, False,
		ButtonPressMask,
		GrabModeAsync, GrabModeAsync, None, None);

//SetLayoutManager(new OHorizontalLayout(this));
//printf("OXColorTrigger::OXColorTrigger [%d][%6x]\n",num,color);
id=num;
_msgObject=0;
colorCell = new OXColorCell(this,color);
AddFrame(colorCell,new OLayoutHints(LHINTS_CENTER_Y,2,0,0,0));
//colorCell->Associate(this);
AddFrame(new OXVertical3dLine(this),
	 new OLayoutHints(LHINTS_EXPAND_Y|LHINTS_CENTER_X, 3, 1, 1, 1));

const OPicture *pic=_client->GetPicture("small-arrow-down.xpm",small_arrow_down);
if(!pic){
	printf("Invalid Pix\n");
	exit(1);
	}
arrow = new OXIcon(this,pic,6,6);
AddFrame(arrow,new OLayoutHints(LHINTS_CENTER_Y|LHINTS_CENTER_X,1,2,1,1));
MapSubwindows();
MapWindow();
Layout();

}
int OXColorTrigger::ProcessMessage(OMessage *msg){
  OContainerMessage *cmsg = (OContainerMessage *) msg;
//printf("OX16ColorTrigger::ProcessMessage(%d,%d,%d,%d)\n",
//	msg->_msg, msg->_submsg, msg->_parm1, msg->_parm2);

  switch(msg->type){
    case MSG_CONTAINER:
      switch(msg->action){
	case MSG_SELCHANGED:
		{
		OContainerMessage msg1(MSG_CONTAINER,MSG_SELCHANGED,id,cmsg->button);
		SendMessage(_msgObject, &msg1);
		colorCell->SetColor(IRCcolors[cmsg->button]);
//		UnmapWindow();
		}
		return true;
	  // cmsg->button is the new color;
	  break;
	default:
	  break;
      }
    case MSG_BUTTON:
      switch (msg->action) {
        case MSG_CLICK:
		break;
		}
    
  }

}

int OXColorTrigger::HandleButton(XButtonEvent *event){

    if (event->type == ButtonPress) {

        int ax, ay;
	Window wdummy;
//	static int c=0;

	OX16ColorDialog *cd = new OX16ColorDialog(_client->GetRoot());
	XTranslateCoordinates(GetDisplay(), _id, _client->GetRoot()->GetId(),
                          0, _h, &ax, &ay, &wdummy);
	cd->Associate(this);
    	cd->PlacePopup(ax, ay, cd->GetDefaultWidth(), cd->GetDefaultHeight());

//	tp->Move(_x,_h+2);
//	printf("Color Selected %d\n",c);
//        OMessage msg(XC_CONTAINER,CT_SELCHANGED,id,c);
//        SendMessage(_msgObject, &msg);

//	if( (c>-1) && (c<16))
//		colorCell->SetColor(IRCcolors[c]);
    }
    if (event->type == ButtonRelease) {
    }
    return False;

}
//------------------------------------------------------------------------------
void OXColorCell::DrawBorder(){
ClearWindow();
DrawLine(_blackGC,_w-1,0,0,0);
DrawLine(_blackGC,0,0,0,_h-1);
DrawLine(_blackGC,0,_h-1,_w-1,_h-1);
DrawLine(_blackGC,_w-1,_h-1,_w-1,0);
}
void OXColorCell::SetColor(unsigned long _color){
color=_color;
SetBackgroundColor(_color);
DrawBorder();
}
OXColorCell::OXColorCell(const OXWindow *p,unsigned long c ):
  OXFrame(p,22,14,OWN_BKGND,c),
 color(c) { }
