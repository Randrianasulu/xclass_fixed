/**************************************************************************

    This file is part of rx320, a control program for the Ten-Tec RX320
    receiver. Copyright (C) 2000, 2001, Hector Peraza.

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

#ifndef __TOOLBARDEF_H
#define __TOOLBARDEF_H

#include <xclass/OXToolBar.h>

#include "tb-open.xpm"
#include "tb-save.xpm"
#include "tb-print.xpm"

//---------- toolbar button definitions

SToolBarData tb_data[] = {
  { "tb-open.xpm",  tb_open_xpm,  "Open File",         BUTTON_NORMAL, M_FILE_OPEN,  NULL },
  { "tb-save.xpm",  tb_save_xpm,  "Save Marked Pages", BUTTON_NORMAL, M_FILE_SAVE,  NULL },
  { "tb-print.xpm", tb_print_xpm, "Print",             BUTTON_NORMAL, M_FILE_PRINT, NULL },
  { "",             NULL,         0,                   0,             -1,           NULL },
  { NULL,           NULL,         NULL,                0,             0,            NULL }
};

#endif  // __TOOLBARDEF_H
