#include <xclass/OXFileDialog.h>
#include <xclass/OXTransientFrame.h>
#include <stdlib.h>

//#include <fOX/OXFavorFileDialog.h>
#include <xclass/OXButton.h>
#include <xclass/OXLabel.h>

#include "OTcp.h"

#define ID_DCC_SAVE		2001
#define ID_DCC_SAVE_AS		2002
#define ID_DCC_REJECT		2003
#define ID_DCC_IGNORE		2004
#define ID_DCC_CANCEL		2005

//-----------------------------------------------------------------------------

class OXProgressBar;
class OFileHandler;

class DCCFileConfirm:public OXTransientFrame{
public:
DCCFileConfirm(const OXWindow *p, const OXWindow *main, char*nick, 
		char *filename, char *size,OFileInfo *retn,int *reti);
~DCCFileConfirm();
virtual int ProcessMessage(OMessage *msg);
virtual void CloseWindow();

protected:

OFileInfo *fi;
int *iret;
char *name;
OLayoutHints *L1,*L2;
OXButton *_ok,*_saveas,*_reject,*_ignore;
};
//-----------------------------------------------------------------------------
class OXDCCFile:public OXMainFrame{
public:
  OXDCCFile(const OXWindow *p, char*nick, char *filename, char*ip,
  		char *port,char *size=0);
  OXDCCFile(const OXWindow *p, char*nick, char *filename)
  :OXMainFrame(p,10,10) {};
  ~OXDCCFile();
  virtual int HandleFileEvent(OFileHandler *fh, unsigned int mask);
  virtual int  ProcessMessage(OMessage *msg);

protected:

bool _OpenFile(char *name);
bool _FetchSomeData();

OTcp *_tcp;
OFileInfo fi;
OFileHandler *_fh;
OXProgressBar *_prog;
OXButton *_cancel;
OXLabel *_t1,*_t2, *_t3;
OLayoutHints *L1,*L2;

int _file;
bool _coned;
bool _serverSocket;
unsigned long bytesread;
unsigned long filesize;
char *_filename;
char _dir[PATH_MAX];

};

