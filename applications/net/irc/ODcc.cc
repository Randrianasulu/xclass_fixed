
class DCCItem {
public:
  DCCItem(char *nic, char *usr) : nick(strdup(nic)), user(strdup(usr)) {};

protected:
  char *nick;
  char *user;
  unsigned long bsent;
  unsigned long brecv;
};

ODCCManager {
public:
  HandleRequest(char *from, char *type, char *rest);
  RequestChar(char *user);
  OfferFile(char *user, char *file);
};

class OXDCC : public OXMainFrame {

  friend class OXIrc;

protected:
  OTcp *tcp;
  char *host;
  unsigned long port;
  unsigned long flags;
  DCCItem *dccitem;
};

class DCCChat : public DCCItem {
public:
  DCCFile(const char *nic, const char *usr) : DCCItem(nic, usr) {};
};

class DCCFile : public DCCItem {
public:
  DCCFile(const char *nic, const char *usr,
          const char *file, unsigned long size) : 
    DCCItem(nic, usr), filename(strdup(file)), filesize(size) {};

protected:
  char *filename;
  unsigned long filesize;
};
