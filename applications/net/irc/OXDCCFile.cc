#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xclass/utils.h>
#include <xclass/OXMsgBox.h>
#include <xclass/OXLabel.h>
#include <xclass/OXFont.h>
#include <xclass/OString.h>
#include <xclass/OFileHandler.h>
#include <xclass/OXProgressBar.h>

#include "OXDCCFile.h"


extern char *filetypes[];

//----------------------------------------------------------------------

DCCFileConfirm::DCCFileConfirm(const OXWindow *p, const OXWindow *main,
                               const char *nick, char *filename, char *size,
                               OFileInfo *retn, int *reti) :
  OXTransientFrame(p, main, 10, 10) {

    int width = 0, height = 0;

    if (retn) fi = retn;
    if (reti) iret = reti;

    if (iret) *iret = ID_DCC_REJECT;

    L1 = new OLayoutHints(LHINTS_CENTER_Y | LHINTS_EXPAND_X, 3, 3, 0, 0);
    L2 = new OLayoutHints(LHINTS_CENTER_X | LHINTS_CENTER_Y);

    OString *temp = new OString("");
    temp->Append(nick);
    temp->Append(" wishes to send you ");
    temp->Append(filename);
    temp->Append("\nwhich is ");
    temp->Append(size);
    temp->Append(" bytes long.");

    OXLabel *lb = new OXLabel(this, temp);
    lb->SetTextJustify(TEXT_JUSTIFY_CENTER);
    AddFrame(lb, L2);

    // button frame
    OXCompositeFrame *btnf = new OXHorizontalFrame(this, 60, 20, FIXED_WIDTH);

    // ok button 
    _ok = new OXTextButton(btnf, new OHotString("&Save"), ID_DCC_SAVE);
    btnf->AddFrame(_ok, L1);
    _ok->Associate(this);

    width = max(width, _ok->GetDefaultWidth());

    // saveas button
    _saveas = new OXTextButton(btnf, new OHotString("S&ave As..."), ID_DCC_SAVE_AS);
    _saveas->Associate(this);
    btnf->AddFrame(_saveas, L1);

    width = max(width, _saveas->GetDefaultWidth());

    // reject button
    _reject = new OXTextButton(btnf, new OHotString("&Reject"), ID_DCC_REJECT);
    btnf->AddFrame(_reject, L1);
    _reject->Associate(this);

    width = max(width, _reject->GetDefaultWidth());

    // ignore button
    _ignore = new OXTextButton(btnf, new OHotString("&Ignore"), ID_DCC_IGNORE);
    btnf->AddFrame(_ignore, L1);
    _ignore->Associate(this);

    width = max(width, _ignore->GetDefaultWidth());

    // add button frame and resize it allowing a 20 pixel border
    AddFrame(btnf, new OLayoutHints(LHINTS_BOTTOM | LHINTS_CENTER_X, 0, 0, 0, 5));
    btnf->Resize(((width + 6) * 4), btnf->GetDefaultHeight());

    _ok->Resize(width, _ok->GetDefaultHeight());
    _saveas->Resize(width, _saveas->GetDefaultHeight());
    _reject->Resize(width, _reject->GetDefaultHeight());
    _ignore->Resize(width, _ignore->GetDefaultHeight());

    //_focusMgr->SetFocusOwner(_ok);
    SetFocusOwner(_ok);

    MapSubwindows();
    width  = GetDefaultWidth() + 20;
    height = GetDefaultHeight() + 20;

    Resize(width, height);

    CenterOnParent();

    //---- make the dialog box non-resizable

    SetWMSize(width, height);
    SetWMSizeHints(width, height, width, height, 0, 0);

    SetWindowName("DCC File Confirm");
    SetIconName("DCC File Confirm");
    SetClassHints("fOXIrc", "dialog");

    MapWindow();

    if (filename) {
      name = StrDup(filename);
      if (fi->filename) delete[] fi->filename;
      fi->filename = StrDup(filename);
    }

    _client->WaitFor(this); 
}

int DCCFileConfirm::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  switch (msg->action) {
    case MSG_CLICK:
      switch (msg->type) {
        case MSG_BUTTON:
          switch (wmsg->id) {
            case ID_DCC_SAVE:
              if (iret) *iret = ID_DCC_SAVE;
              CloseWindow();
              break;

            case ID_DCC_SAVE_AS:
              {
              OFileInfo f2;
              f2.filename = StrDup(fi->filename);
              f2.file_types = filetypes;
              f2.MimeTypesList = NULL;
              if (fi->filename) f2.filename = StrDup(fi->filename);
              new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &f2);
              if (f2.filename) {
                if (fi->filename) delete[] fi->filename;
                fi->filename = StrDup(f2.filename);
                if (fi->ini_dir) delete[] fi->ini_dir;
                fi->ini_dir = StrDup(f2.ini_dir);
                if (iret) *iret = ID_DCC_SAVE;
                CloseWindow();
              }
              }
              return true;
              break;

            case ID_DCC_REJECT:
            case ID_DCC_IGNORE:
              if (iret) *iret = wmsg->id;
              if (fi->filename) delete[] fi->filename;
              fi->filename = 0;
              CloseWindow();
              break;

            default:
              break;
          }

        default:
          break;
      }
      break;

    default:
      break;
  }

  return true;
}

DCCFileConfirm::~DCCFileConfirm() {
  delete L1;
  delete L2;
}


//----------------------------------------------------------------------

OXDCCFile::OXDCCFile(const OXWindow *p, const char *nick, char *filename,
                     char *ip, char *port, char *size, int *retc) :
  OXMainFrame(p, 100, 100) {

    char name[PATH_MAX];
    int  ste, wid;

    _retc = retc;
    if (_retc) *_retc = ID_DCC_REJECT;

    filesize = strtoul(size, NULL, 10);
    _tcp = new OTcp();
    _coned = false;
    _fh = NULL;
    _file = -1;
    _serverSocket = false;
    bytesread = 0;

    if (fi.filename) delete[] fi.filename;
    if (filename)
      fi.filename = StrDup(filename);
    else
      fi.filename = 0;

    fi.file_types = filetypes;
    fi.MimeTypesList = NULL;

    if (filename)
      _filename = StrDup(filename);
    else
      _filename = 0;

    if (getcwd(_dir, PATH_MAX))
      fi.ini_dir = StrDup(_dir);
    else
      fi.ini_dir = StrDup("/");

    OString *str = new OString("Saving as: ");
    str->Append(_filename);

    L1 = new OLayoutHints(LHINTS_NORMAL | LHINTS_EXPAND_X, 2, 2, 2, 2);
    L2 = new OLayoutHints(LHINTS_EXPAND_X | LHINTS_CENTER_Y, 2, 2, 2, 2);

    _t1 = new OXLabel(this, str);
    wid = _t1->GetDefaultWidth();

    _t2 = new OXLabel(this, new OString("Bytes Recieved:"));
    _t2->SetTextAlignment(TEXT_LEFT);

    wid = max(wid, _t2->GetDefaultWidth());
    _t3 = new OXLabel(this, new OString("Bytes Remaining:"));
    _t3->SetTextAlignment(TEXT_LEFT);

    wid = max(wid, _t3->GetDefaultWidth());

    AddFrame(_t1, L1);
    AddFrame(_t2, L1);
    AddFrame(_t3, L1);

    _prog = new OXProgressBar(this, 150, 20);
    _prog->SetRange(0, strtoul(size, NULL, 10));
    _prog->SetPosition(0);
    _prog->ShowPercentage(true);
    AddFrame(_prog, L2);

    _cancel = new OXTextButton(this, new OHotString("&Cancel"), -1);
    _cancel->Associate(this);
    AddFrame(_cancel, new OLayoutHints(LHINTS_CENTER_X, 2, 2, 2, 2));

    Resize(wid+80, GetDefaultHeight()+20);

    SetWindowName("DCC File");
    SetIconName("DCC File");
    SetClassHints("XDCC", "XDCC");

    MapSubwindows();
    Layout();
    MapWindow();

    new DCCFileConfirm(_client->GetRoot(), this, nick, filename, size,
                       &fi, &ste);

    if (_retc) *_retc = ste;

    if (ste == ID_DCC_SAVE) {
      printf("File name is %s\n", fi.filename);
      printf("Path is %s\n", fi.ini_dir);

      sprintf(name, "%s/%s", fi.ini_dir, fi.filename);
      if (_OpenFile(name)) {
        _tcp->Connect(strtoul(ip, NULL, 10), atoi(port), true);
        _fh = new OFileHandler(this, _tcp->GetFD(), XCM_WRITABLE | XCM_EXCEPTION);
        OString *str = new OString("Saving as: ");
        str->Append(fi.filename);
        _t1->SetText(str);
      } else {
        CloseWindow();
        if (_retc) *_retc = ID_DCC_REJECT;
      }
    } else {
      CloseWindow();
    }
}

OXDCCFile::~OXDCCFile() {
  if (_tcp) delete _tcp;
  if (_fh) delete _fh;
  _fh = 0;
  delete L1;
  delete L2;
}

int OXDCCFile::ProcessMessage(OMessage *msg) {
  OWidgetMessage *wmsg = (OWidgetMessage *) msg;

  switch (msg->action) {
    case MSG_CLICK:
      switch (msg->type) {
        case MSG_BUTTON:
          CloseWindow();  // cancel pressed...
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
  return false;
}

/*
bool OXDCCFile::_ConfirmSaveAs(char *filename) {
  // always open the file in append mode,
  // perhaps we should ask for this too...

  fi.MimeTypesList = NULL;
  fi.file_types = filetypes;
  if (fi.filename) delete[] fi.filename;
  fi.filename = StrDup(filename);
  new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &fi);
  if (fi.filename) {
    if (_OpenFile(fi.filename)) return true;
  }
  return false;
}
*/

/*
int OXDCCFile::HandleFileEvent(OFileHandler *fh, unsigned int mask) {
  if (fh != _fh) return false;

  delete _fh;
  _fh = NULL;
}
*/

bool OXDCCFile::_OpenFile(char *name) {
  struct stat st;
  char *fname = name;
  OFileInfo f2;

  while (stat(fname, &st) == 0) {
    int rt;
    OString stitle("Save");
    OString smsg("A file with the same name already exists. Overwrite?");
    new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                 MB_ICONQUESTION, ID_YES | ID_NO, &rt);

    if (rt == ID_YES) break;

    f2.filename = StrDup(name);
    f2.file_types = filetypes;
    f2.MimeTypesList = NULL;
    new OXFileDialog(_client->GetRoot(), this, FDLG_SAVE, &f2);
    if (!f2.filename) return false;
    fname = f2.filename;
  }

  _file = open(fname, O_CREAT | O_WRONLY | O_TRUNC);

  if (_file < 0) {
    OString stitle("Save");
    OString smsg("Failed to create file: ");
    smsg.Append(strerror(errno));
    new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                 MB_ICONSTOP, ID_OK);
    return false;
  }

  return true;
}

int OXDCCFile::HandleFileEvent(OFileHandler *fh, unsigned int mask) {
  if (fh != _fh) return False;

/*
  if (_serverSocket) {
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
        if (!_serverSocket) _FetchSomeData();
        break;
    }
//  }

  return True;
}

bool OXDCCFile::_FetchSomeData() {
  char buf[TCP_BUFFER_LENGTH], char1[TCP_BUFFER_LENGTH];
  unsigned long bytestemp;

  int size = _tcp->BinaryReceive((char *) &buf, TCP_BUFFER_LENGTH);

  if (size > 0) {
    write(_file, buf, size);
    bytesread += size;
    bytestemp = htonl(bytesread);

    sprintf(char1, "Bytes Recieved: %ld", bytesread);
    _t2->SetText(new OString(char1));
    
    sprintf(char1, "Bytes Remaining: %ld", filesize - bytesread);
    _t3->SetText(new OString(char1));

    _prog->SetPosition(bytesread);
    if (write(_tcp->GetFD(), (char *) &bytestemp, sizeof(unsigned long)) < 0) {
      OString stitle("Reading File");
      OString smsg("Remote end close connection ");
      new OXMsgBox(_client->GetRoot(), this, &stitle, new OString(&smsg),
                   MB_ICONSTOP, ID_OK);
      return false;
    }

    if (_fh) delete _fh;
    _fh = new OFileHandler(this, _tcp->GetFD(), XCM_READABLE);

  } else {
    if (bytesread != filesize) {
      OString stitle("Error Reading File");
      OString smsg("File size is different than reported");
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
