/**************************************************************************

    This file is part of xclass.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

#ifndef __OXTOOLBAR_H
#define __OXTOOLBAR_H


#include <xclass/utils.h>
#include <xclass/OXClient.h>
#include <xclass/OXCompositeFrame.h>
#include <xclass/OXWidget.h>
#include <xclass/OXPictureButton.h>
#include <xclass/OString.h>


struct SToolBarData {
  char *pixmap_name;
  char **pixmap_data;
  char *tip_text;
  int  type;
  int  id;
  OXButton *button;
};


//----------------------------------------------------------------------

class OXToolBar : public OXHorizontalFrame {
public:
  OXToolBar(const OXWindow *p, int x = 10, int y = 10,
            unsigned int options = CHILD_FRAME);
  virtual ~OXToolBar();

  void AddButton(OXButton *b, char *tip_text = NULL);
  OXButton *AddButton(const OPicture *pic, char *tip_text, int type, int id);
  void AddButtons(SToolBarData *data);
  void AddSeparator();

  OXButton *GetButtonById(int id);

protected:
  int _spacing;
};


#endif   // __OXTOOLBAR_H
