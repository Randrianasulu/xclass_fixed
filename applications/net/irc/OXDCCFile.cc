#include "OXDCCFile.h"
#include <xclass/utils.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXLabel.h>
#include <xclass/OString.h>
#include <xclass/OFileHandler.h>
#include <xclass/OXProgressBar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h>

extern char *filetypes[];
//--------------
//-----------------------------------------------------------------------------
//--------------
DCCFileConfirm::DCCFileConfirm(const OXWindow *p, const OXWindow *main, char*nick, 
		char *filename, char *size,OFileInfo *retn,int *reti):
  OXTransientFrame(p, main, 10, 10) {

int ax, ay, width = 0, height = 0;
unsigned int root_w, root_h, dummy;
Window wdummy;

if(retn) fi=retn;
if(reti) iret=reti;
// Build the labels..
L1 = new OLayoutHints(LHINTS_CENTER_Y | LHINTS_EXPAND_X, 3, 3, 0, 0);
L2 = new OLayoutHints(LHINTS_CENTER_X | LHINTS_CENTER_Y);
OString *temp1=new OString("");
temp1->Append(nick);
temp1->Append(" Wishes to send you ");
temp1->Append(filename);
OString *temp2=new OString(" Which is ");
temp2->Append(size);
temp2->Append(" Bytes long");
AddFrame(new OXLabel(this,temp1),L2);
AddFrame(new OXLabel(this,temp2),L2);

// button frame
OXCompositeFrame *ButtonFrame = new OXHorizontalFrame(this, 60, 20, FIXED_WIDTH);

// ok button 
_ok = new OXTextButton(ButtonFrame,new OHotString("&Save"),ID_DCC_SAVE);
ButtonFrame->AddFrame(_ok,L1);
_ok->Associate(this);
width = max(width,_ok->GetDefaultWidth());
// saveas button
_saveas = new OXTextButton(ButtonFrame,new OHotString("S&ave As"),ID_DCC_SAVE_AS);
_saveas->Associate(this);
ButtonFrame->AddFrame(_saveas,L1);
width = max(width,_saveas->GetDefaultWidth());
// reject button
_reject = new OXTextButton(ButtonFrame,new OHotString("&Reject"),ID_DCC_REJECT);
ButtonFrame->AddFrame(_reject,L1);
_reject->Associate(this);
width = max(width,_reject->GetDefaultWidth());
//ignore button
_ignore = new OXTextButton(ButtonFrame,new OHotString("&Ignore"),ID_DCC_IGNORE);
ButtonFrame->AddFrame(_ignore,L1);
_ignore->Associate(this);
width = max(width,_ignore->GetDefaultWidth());
// add button frame and resize it allowing a 20 pixel border
AddFrame(ButtonFrame,new OLayoutHints(LHINTS_BOTTOM | LHINTS_CENTER_X, 0, 0, 0, 5));
ButtonFrame->Resize( ((width+6)* 4), ButtonFrame->GetDefaultHeight());

_ok->Resize(width,_ok->GetDefaultHeight());
_saveas->Resize(width,_saveas->GetDefaultHeight());
_reject->Resize(width,_reject->GetDefaultHeight());
_ignore->Resize(width,_ignore->GetDefaultHeight());

//_focusMgr->SetFocusOwner(_ok);
SetFocusOwner(_ok);

MapSubwindows();
width  = GetDefaultWidth()+20;
height = GetDefaultHeight()+20;

Resize(width, height);
if (main) {
      XTranslateCoordinates(GetDisplay(),
                            main->GetId(), GetParent()->GetId(),
                            (((OXFrame *) main)->GetWidth() - _w) >> 1,
                            (((OXFrame *) main)->GetHeight() - _h) >> 1,
                            &ax, &ay, &wdummy);
    } else {
      XGetGeometry(GetDisplay(), _client->GetRoot()->GetId(), &wdummy,
                   &ax, &ay, &root_w, &root_h, &dummy, &dummy);
      ax = (root_w - _w) >> 1;
      ay = (root_h - _h) >> 1;
    }

    Move(ax, ay);
    SetWMPosition(ax, ay);

    //---- make the dialog box non-resizable

    SetWMSize(width, height);
    SetWMSizeHints(width, height, width, height, 0, 0);

SetWindowName("DCC File Confirm");
SetIconName("DCC File Confirm");
SetClassHints("MsgBox", "MsgBox");
MapWindow();

if(filename) {
	 name=StrDup(filename);
	 if(fi->filename)
	 	delete []fi->filename;
	 fi->filename=StrDup(filename);
 	}
_client->WaitFor(this); 
}
//-----------------------------------------------------------------------------
int DCCFileConfirm::ProcessMessage(OMessage *msg){
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  switch(msg->action) {
  case MSG_CLICK:
    switch(msg->type) {
    case MSG_BUTTON:
	switch(wmsg->id){
 	case ID_DCC_SAVE:
	      if(iret) *iret=ID_DCC_SAVE;
	      CloseWindow();
	      break;
	case ID_DCC_SAVE_AS:
		{
		OFileInfo f2;
		f2.filename = StrDup(fi->filename);
		f2.file_types = filetypes;
		f2.MimeTypesList = NULL;
		if(fi->filename) f2.filename = StrDup(fi->filename);
         	new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &f2);
			if (!f2.filename) {
				return true;
				}
			else{
			  if(fi->filename) delete[] fi->filename;
			  fi->filename = StrDup(f2.filename);
			  if(fi->ini_dir) delete[] fi->ini_dir;
			  fi->ini_dir = StrDup(f2.ini_dir);
			  if(iret) *iret=ID_DCC_SAVE;
			  CloseWindow();
			  
			}
		}
	      return true;
	      break;
	case ID_DCC_REJECT:
	case ID_DCC_IGNORE:
	      if(iret) *iret=ID_DCC_IGNORE;
	      if(fi->filename) delete fi->filename;
	      fi->filename=0;
	      CloseWindow();
//	      delete this;
//	      return true;
	      break;
	default:
	      break;
	}
    default:
	break;
    }
  default:
    break;
  }
return true;
}
//----------------------
DCCFileConfirm::~DCCFileConfirm() {
  delete L1;
  delete L2;
}

//--------------
//-----------------------------------------------------------------------------
//--------------

OXDCCFile::OXDCCFile(const OXWindow *p, char*nick, char *filename,char*ip,
  		char *port,char *size):OXMainFrame(p,100,100){

//--------------

char name[PATH_MAX];
int ste,wid;

filesize = strtoul(size, NULL, 10);
_tcp = new OTcp();
_coned = false;
_file = 0;
_serverSocket = false;
bytesread = 0;

if(fi.filename) delete fi.filename;
if(filename)
	fi.filename = StrDup(filename);
else
	fi.filename = 0;

fi.file_types = filetypes;
fi.MimeTypesList = NULL;

if(filename) _filename = StrDup(filename);
else _filename = 0;

if(getcwd(_dir,PATH_MAX))
	fi.ini_dir = StrDup(_dir);
else
	fi.ini_dir = StrDup("/");
OString *str = new OString("Saving as :");
str->Append(_filename);

L1 = new OLayoutHints(LHINTS_NORMAL | LHINTS_EXPAND_X, 2, 2, 2, 2);
L2 = new OLayoutHints(LHINTS_EXPAND_X | LHINTS_CENTER_Y,2,2,2,2);

_t1 = new OXLabel(this,str);
wid=_t1->GetDefaultWidth();

_t2 = new OXLabel(this,new OString("Bytes Recieved:"));
_t2->SetTextAlignment(TEXT_LEFT);
wid = max(wid,_t2->GetDefaultWidth());
_t3 = new OXLabel(this,new OString("Bytes Remaining:"));
_t3	->SetTextAlignment(TEXT_LEFT);
wid = max(wid,_t3->GetDefaultWidth());

AddFrame(_t1,L1);
AddFrame(_t2,L1);
AddFrame(_t3,L1);


_prog = new OXProgressBar(this,150,20);
_prog->SetRange(0,strtoul(size, NULL, 10));
_prog->SetPosition(0);
_prog->ShowPercentage(true);
AddFrame(_prog,L2);

_cancel = new OXTextButton(this,new OHotString("&Cancel"),-1);
_cancel->Associate(this);
AddFrame(_cancel,new OLayoutHints(LHINTS_CENTER_X,2,2,2,2));

Resize(wid+80,GetDefaultHeight()+20);

SetWindowName("DCC File");
SetIconName("DCC File");
SetClassHints("XDCC", "XDCC");

MapSubwindows();
MapWindow();
Layout();

new DCCFileConfirm(_client->GetRoot(), this, nick, filename, size, &fi,&ste);
if(ste==ID_DCC_SAVE){
	printf("File name is %s\n",fi.filename);
	printf("Path is %s\n",fi.ini_dir);

	sprintf(name,"%s/%s",fi.ini_dir,fi.filename);
	if(_OpenFile(name)){
		_tcp->Connect(strtoul(ip, NULL, 10),atoi(port),true);
		_fh = new OFileHandler(this, _tcp->GetFD(), XCM_WRITABLE | XCM_EXCEPTION);
		OString *str = new OString("Saving as :");
		str->Append(fi.filename);
		_t1->SetText(str);
		}
} else
 {
  CloseWindow();
  }
}
//-----------------------------------------------------------------------------
OXDCCFile::~OXDCCFile(){
if (_tcp) delete _tcp;
if(_fh) delete _fh;
_fh = 0;
delete L1;
delete L2;

}
//-----------------------------------------------------------------------------
int OXDCCFile::ProcessMessage(OMessage *msg){
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  switch(msg->action) {
    case MSG_CLICK:
      switch(msg->type) {
    	case MSG_BUTTON:
    	default:
    		break;
	}
	default:
	break;
    }
return false;
}
//-----------------------------------------------------------------------------
/* bool OXDCCFile::_ConfirmSaveAs(char *filename){
  // always open the file in append mode,
  // perhaps we should ask for this too...

  fi.MimeTypesList = NULL;
  fi.file_types = filetypes;
  if(fi.filename) delete fi.filename;
  fi.filename = StrDup(filename);
  new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &fi);
  if (fi.filename) {
	if(_OpenFile(fi.filename))
		return true;
	}
return false;
};
 */
//-----------------------------------------------------------------------------
/*int OXDCCFile::HandleFileEvent(OFileHandler *fh, unsigned int mask){
if(fh != _fh){
	return false;
	} else {


	}
delete _fh; _fh = NULL;
}*/
//-----------------------------------------------------------------------------
bool OXDCCFile::_OpenFile(char *name){

struct stat st;
char *fname = name;
OFileInfo f2;

if(stat(name,&st)==0){
      int rt;
      OString stitle("File Open Error");
      OString smsg("File Exists Overwrite?");
      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                   MB_ICONSTOP, ID_YES|ID_NO,&rt);

      if(rt==ID_NO)
	{
	      f2.filename = StrDup(name);
      	      f2.file_types = filetypes;
	      f2.MimeTypesList = NULL;
	      new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &f2);
	      if (!f2.filename) {
		  return false;
		}
	      fname = f2.filename;
	}
 }

_file = open(fname, O_CREAT|O_WRONLY|O_EXCL , S_IRUSR|S_IWUSR);
/*    if ((!_file)&&(errno == EEXIST)) {
      int rt;
      OString stitle("File Open Error");
      OString smsg("File Exists Overwrite?");
      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                   MB_ICONSTOP, ID_YES|ID_NO,&rt);
      return false;
    }
*/
    if (!_file) {
      OString stitle("File Open Error");
      OString smsg("Failed to open log file: ");
      smsg.Append(strerror(errno));
      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                   MB_ICONSTOP, ID_OK);
      return false;
    }

return true;
}

//-----------------------------------------------------------------------------

int OXDCCFile::HandleFileEvent(OFileHandler *fh, unsigned int mask) {
  if (fh != _fh) {
    return False;
  } else {
/*    if (_serverSocket) {
      if (_fl) { delete _fl; _fl = NULL; }
      OTcp *newconn = new OTcp();
      int ret = newconn->Accept(_dccServer->GetFD());  // write this better...
      delete _dccServer;  // we do not have to continue
                          // listening on that port...
      _dccServer = newconn;
      _dccServer->Associate(this);
      if (ret >= 0) {
        char s[256];

        _fl = new OFileHandler(this, _dccServer->GetFD(), XCM_READABLE);
        sprintf(s, "Connection to %s:%d established",
                    _dccServer->GetAddress(), _dccServer->GetPort());
        Log(s, "green");
        _coned = True;
      } else {
        Log("Connection failed", "red");
      }
      _serverSocket = False;

    } else {
*/
      switch (mask) {
        case XCM_WRITABLE:
          if (!_coned) {
            _coned = True;

            if (_fh) delete _fh;
            _fh = new OFileHandler(this, _tcp->GetFD(), XCM_READABLE);
          }
          break;

        case XCM_READABLE:
	{
	if(!_serverSocket) _FetchSomeData();

	}
          break;
      }
//    }
  }

  return True;
}
//----------------------------------------------------------------------------
bool OXDCCFile::_FetchSomeData(){
	  char buf[TCP_BUFFER_LENGTH],char1[TCP_BUFFER_LENGTH];

          unsigned long bytestemp;

	  int size = _tcp->BinaryReceive((char *)&buf,TCP_BUFFER_LENGTH);
	  if(size>0){
		write(_file,buf,size);
		bytesread += size;
		bytestemp = htonl(bytesread);

		sprintf(char1,"Bytes Recieved: %d",bytesread);
		_t2->SetText(new OString(char1));
		
		sprintf(char1,"Bytes Remaining: %d",filesize-bytesread);
		_t3->SetText(new OString(char1));

		_prog->SetPosition(bytesread);
		if (write(_tcp->GetFD(), (char *)&bytestemp, sizeof(unsigned long)) == -1)
		{
		      OString stitle("Reading File");
		      OString smsg("Remote end close connection ");
		      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                			   MB_ICONSTOP, ID_OK);
		      return false;
		}
               if (_fh) delete _fh;
                _fh = new OFileHandler(this, _tcp->GetFD(), XCM_READABLE);
	  	} else {
		if(bytesread != filesize){
		      OString stitle("Error Reading File");
		      OString smsg("file size is different then reported");
		      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                			   MB_ICONSTOP, ID_OK);
		      return false;
		      }
		close(_file);
                if (_fh) delete _fh;
		_fh = 0;
	        _tcp->Close();
		CloseWindow();
		return true;
		}
return true;
}
