#ifndef __OXSDLIST_H
#define __OXSDLIST_H

#include <xclass/OXSList.h>

class OXSDList : public OXSList {
public:
  OXSDList(Display *dpy, XPointer data) : OXSList(dpy, data) {}

  Bool SetFirst(int id);
  Bool SetLast(int id);
};

#endif  // __OXSDLIST_H