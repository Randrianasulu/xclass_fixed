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

#ifndef __OXDIALOGS_H
#define __OXDIALOGS_H

#include <xclass/utils.h>
#include <xclass/OXTransientFrame.h>

#include "OFreqRecord.h"

class OXTextEntry;
class OXDDListBox;


//---------------------------------------------------------------------

class OXEditStation : public OXTransientFrame {
public:
  OXEditStation(const OXWindow *p, const OXWindow *main,
                OFreqRecord *mime, int *retc = NULL,
                unsigned long options = MAIN_FRAME | VERTICAL_FRAME);
  virtual ~OXEditStation();
  
  virtual void CloseWindow();
  virtual int  ProcessMessage(OMessage *msg);

protected:
  void InitControls();
  void UpdateRecord();

  OXTextEntry *_name, *_freq, *_tstep, *_pbt, *_loc, *_lang,
              *_stime, *_etime, *_notes;
  OXDDListBox *_mode, *_filter, *_agc;
  OLayoutHints *bly, *bfly, *lnly, *lyl, *lyr;

  int *_retc;
  OFreqRecord *_freqRec;
};

#endif  // __OXDIALOGS_H
