#ifndef __OIRC_H
#define __OIRC_H

#include <stdlib.h>
#include <stdio.h>

#include <xclass/OFileHandler.h>
#include "OTcp.h"

//---------------------------------------------------------------------

class OIrc : public OComponent {
public:
  OIrc();
  virtual ~OIrc();

  int  Connect(char *server, int port) 
       const { return _tcp->Connect(server, port, True); }
  void Close() const { _tcp->Close(); }
  void Associate(OComponent * w) { _msgObject = w; }
  int  GetFD() const { return _tcp->GetFD(); }

  int Receive() { return _tcp->Receive(); }
  virtual int ProcessMessage(OMessage *msg);
  OTcp *GetOTcp() { return _tcp; }

protected:
  OTcp *_tcp;
  OComponent *_msgObject;
};

#endif  // __OIRC_H
