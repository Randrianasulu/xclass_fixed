/**************************************************************************

    This file is part of xclass, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Ricky Ralston, Hector Peraza.

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

#ifndef __OXFILEDIALOG_H
#define __OXFILEDIALOG_H

#include <xclass/OXTransientFrame.h>
#include <xclass/OXListView.h>
#include <xclass/OMimeTypes.h>
#include <xclass/OXFileList.h>
#include <xclass/OXFSDDListBox.h>
#include <xclass/OXLabel.h>
#include <xclass/OXIcon.h>
#include <xclass/OXTextButton.h>
#include <xclass/OXPictureButton.h>
#include <xclass/OXTextEntry.h>


#define FDLG_OPEN    0
#define FDLG_SAVE    1
#define FDLG_BROWSE  2


class OFileInfo {
public:
  char *filename;
  char *ini_dir;
  char **file_types;
  OMimeTypes *MimeTypesList;  // temp here...
  OFileInfo();
  ~OFileInfo();
};

class OXFileDialog : public OXTransientFrame {
public:
  OXFileDialog(const OXWindow *p, const OXWindow *main,
               int dlg_type, OFileInfo *file_info);
  virtual ~OXFileDialog();

  virtual int  ProcessMessage(OMessage *msg);
  virtual void CloseWindow();
  virtual int  HandleMapNotify(XMapEvent *event);
  virtual int  HandleKey(XKeyEvent *event);

protected:
  void _GrabAltKey(int keysym);

  int _init;
  OXLabel *_lookin, *_lfname, *_lftypes;
  OXTextEntry *_fname;
  OXDDListBox *_ftypes;
  OXFileSystemDDListBox *_tree_lb;
  OXPictureButton *_cdup, *_newf, *_list, *_details;
  OXTextButton *_ok, *_cancel;
  OXFileList *_fv;
  OXHorizontalFrame *_htop, *_hf, *_hfname, *_hftype;
  OXVerticalFrame *_vf, *_vbf;
  OLayoutHints *_lmain, *_lhl, *_lb1, *_lb2, *_lvf, *_lvbf,
               *_lb, *_lht, *_lht1;
  OFileInfo *_file_info;
  int _dlg_type;
  char *_defName;   // Save dialog should not change to 
  	            // a directory name when double clicked..
};


#endif  // __OXFILEDIALOG_H
