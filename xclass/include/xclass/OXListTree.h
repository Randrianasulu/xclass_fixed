/**************************************************************************

    This file is part of Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

    This code is partially based on Robert W. McMullen's ListTree-3.0
    widget. Copyright (C) 1995.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  
**************************************************************************/
 
#ifndef __OXLISTTREE_H
#define __OXLISTTREE_H

#include <xclass/OXClient.h>
#include <xclass/OXWindow.h>
#include <xclass/OXWidget.h>
#include <xclass/OXCompositeFrame.h>
#include <xclass/OPicture.h>

class OXFont;

class OListTreeItem {
public:
  OListTreeItem(OXClient *_client, char *name,
                const OPicture *opened, const OPicture *closed);
  ~OListTreeItem();

  void Rename(char *new_name);

  OListTreeItem	*parent, *firstchild, *prevsibling, *nextsibling;
  int  open, active;
  char *text;
  int  length, xnode, y, xtext, ytext, height, picWidth;
  const OPicture *open_pic, *closed_pic;

protected:
  OXClient *_client;
};

class OXListTree : public OXFrame, public OXWidget {
public:
  OXListTree(const OXWindow *p, int w, int h,
             unsigned int options, unsigned long back = _defaultDocumentBackground);
  virtual ~OXListTree();

  virtual int HandleButton(XButtonEvent *event);
  virtual int HandleDoubleClick(XButtonEvent *event);
  virtual int HandleExpose(XExposeEvent *event);

  virtual ODimension GetDefaultSize() const
           { return ODimension(_defw, _defh); }

  OListTreeItem *AddItem(OListTreeItem *parent, char *string,
                         const OPicture *open = NULL,
                         const OPicture *closed = NULL);
  void RenameItem(OListTreeItem *item, char *string);
  int  DeleteItem(OListTreeItem *item);
  int  DeleteChildren(OListTreeItem *item);
  int  Reparent(OListTreeItem *item, OListTreeItem *newparent);
  int  ReparentChildren(OListTreeItem *item, OListTreeItem *newparent);

  int  Sort(OListTreeItem *item);
  int  SortSiblings(OListTreeItem *item);
  int  SortChildren(OListTreeItem *item);
  void HighlightItem(OListTreeItem *item);
  void ClearHighlighted();
  void GetPathnameFromItem(OListTreeItem *item, char *path);

  //static int _compare(const void *item1, const void *item2);

  OListTreeItem *GetFirstItem() const { return _first; }
  OListTreeItem *GetSelected() const { return _selected; }
  OListTreeItem *FindSiblingByName(OListTreeItem *item, char *name);
  OListTreeItem *FindChildByName(OListTreeItem *item, char *name);

protected:
  virtual void _DoRedraw();

  void _Draw(int yevent, int hevent);
  int  _DrawChildren(OListTreeItem *item, int x, int y, int xroot);
  void _DrawItem(OListTreeItem *item, int x, int y, int *xroot,
                 int *retwidth, int *retheight);
  void _DrawItemName(OListTreeItem *item);
  void _DrawNode(OListTreeItem *item, int x, int y);

  void _HighlightItem(OListTreeItem *item, int state, int draw);
  void _HighlightChildren(OListTreeItem *item, int state, int draw);
  void _UnselectAll(int draw);

  void _RemoveReference(OListTreeItem *item);
  void _DeleteChildren(OListTreeItem *item);
  void _InsertChild(OListTreeItem *parent, OListTreeItem *item);
  void _InsertChildren(OListTreeItem *parent, OListTreeItem *item);
  int  _SearchChildren(OListTreeItem *item, int y, int findy,
                       OListTreeItem **finditem);
  OListTreeItem *OXListTree::_FindItem(int findy);

  OListTreeItem *_first, *_selected;
  int _hspacing, _vspacing, _indent, _margin, _last_y;
  unsigned int _grayPixel;
  GC _drawGC, _lineGC, _highlightGC;
  const OXFont *_font;
  int _th, _ascent, _defw, _defh;
  int _exposeTop, _exposeBottom;
};


#endif  // __OXLISTTREE_H
