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

#include "OXSMeter.h"


#define S_MAX       80
#define BAR_WIDTH   7    // bar thickness
#define PEAK_SIZE   4    // size of the peak mark

#define PEAK_HOLD   5
#define PEAK_AVG    6


//----------------------------------------------------------------------

OXSMeter::OXSMeter(const OXWindow *p, int orientation) :
  OXFrame(p, 1, 1) {
    XGCValues gcval;
    unsigned int gm;

    _orien = orientation;

    _sfont = _client->GetFont("Helvetica -8");
    _dBfont = _client->GetFont("Helvetica -12 bold");

    gm = GCForeground | GCLineWidth | GCLineStyle | GCFont |
         GCGraphicsExposures;
    gcval.foreground = _client->GetColorByName("#00ffff");
    gcval.line_width = 0;
    gcval.line_style = LineSolid;
    gcval.font = _sfont->GetId();
    gcval.graphics_exposures = False;
    _barGC = new OXGC(GetDisplay(), _id, gm, &gcval);

    gcval.foreground = _client->GetColorByName("#ff0000");
    _maxGC = new OXGC(GetDisplay(), _id, gm, &gcval);

    _sval = _smax = S_MAX;
    _hold = 0;
    _avg = PEAK_AVG;
    _pkhold = PEAK_HOLD;
}

OXSMeter::~OXSMeter() {
  delete _barGC;
  delete _maxGC;
  _client->FreeFont(_sfont);
}

ODimension OXSMeter::GetDefaultSize() const {
  int w, h;

  if (_orien == S_VERTICAL) {
    w = 25 + BAR_WIDTH;
    h = max(40, _h);
  } else {
    w = max(50, _w);
    h = 10 + BAR_WIDTH + 2 + _sfont->TextHeight();
  }

  return ODimension(w, h);
}

void OXSMeter::SetPeakHold(unsigned int hold) {
  _pkhold = hold;
}

void OXSMeter::SetPeakAverage(unsigned int avg) {
  _avg = avg;
  if (_avg == 0) _avg = 1;
}

//----------------------------------------------------------------------

// 0..S_MAX

void OXSMeter::SetS(unsigned int sval) {
  _sval = sval;
  if (_sval > S_MAX) _sval = S_MAX;
  if (_sval > _smax) {
    _smax = _sval;
    _hold = _pkhold;
  } else {
    if (_hold == 0)
      _smax = ((_avg - 1) * _smax + _sval) / _avg;
    else
      --_hold;
  }
  NeedRedraw(True);
}

//static char *label[6] = { "0", "36", "50", "60", "72", "80" };
static char *label[6] = { "0", "16", "32", "48", "64", "80" };

void OXSMeter::_DoRedraw() {
  OXFrame::_DoRedraw();

  OFontMetrics fm;
  _sfont->GetFontMetrics(&fm);

  _barGC->SetFont(_sfont->GetId());

  if (_orien == S_VERTICAL) {
    int hm = PEAK_SIZE;
    int hs = _h - 20 - (hm + 1);  // height of the "S" scale

    int sp = _sval * hs / S_MAX;
    int mp = _smax * hs / S_MAX;

    FillRectangle(_barGC->GetGC(), 20, _h-10-sp, BAR_WIDTH, sp+1);
    FillRectangle(_maxGC->GetGC(), 20, _h-10-mp-(hm+1), BAR_WIDTH, hm);

    int n = 6;
    for (int i = 0; i < n; ++i) {
      int y = _h-10 - i * (_h-20) / (n-1);
      DrawLine(_barGC->GetGC(), 15, y, 17, y);
      DrawString(_barGC->GetGC(),
                 14 - _sfont->XTextWidth(label[i]), y + fm.ascent / 2,
                 label[i], strlen(label[i]));
    }
  } else {
    int dbw = _dBfont->XTextWidth("dB");
    int wm = PEAK_SIZE;  // width of the "max" mark
    int w = _w - 20 - dbw;
    int ws = w - (wm + 1);  // width of the "S" scale

    int sp = _sval * ws / S_MAX;
    int mp = _smax * ws / S_MAX;

    FillRectangle(_barGC->GetGC(), 5, 5, sp+1, BAR_WIDTH);
    FillRectangle(_maxGC->GetGC(), 5+mp+2, 5, wm, BAR_WIDTH);

    int n = 6;
    int y = 5 + BAR_WIDTH + 2;
    for (int i = 0; i < n; ++i) {
      int x = 5 + i * w / (n-1);
      DrawLine(_barGC->GetGC(), x, y, x, y+2);
      DrawString(_barGC->GetGC(),
                 x - _sfont->XTextWidth(label[i]) / 2, y+2 + fm.ascent,
                 label[i], strlen(label[i]));
    }

    _barGC->SetFont(_dBfont->GetId());
    DrawString(_barGC->GetGC(), _w - dbw, y+2 + fm.ascent, "dB", 2);
  }
}
