/**************************************************************************
 
    This file is part of Xclass95, a Win95-looking GUI toolkit.
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

#include <stdio.h>
#include <string.h>

#include <xclass/OXCompositeFrame.h>
#include <xclass/ODimension.h>
#include <xclass/OResourcePool.h>


#include "OXDiffView.h"
#include "OXDiff.h"

//----------------------------------------------------------------------

OXDiffFrame::OXDiffFrame(OXWindow *parent, int x, int y,
                         unsigned int options, unsigned long back) :
  OXFrame(parent, x, y, options, back) {
    XGCValues gcv;
    int mask = GCFont | GCForeground | GCBackground;

    _font = _client->GetFont("Courier -12");

    _th = _font->TextHeight();

    SetBackgroundColor(back);

    unsigned long black = GetResourcePool()->GetDocumentFgndColor();
    gcv.foreground = black;
    gcv.background = back;
    gcv.font = _font->GetId();
    _gc = XCreateGC(GetDisplay(), _id, mask, &gcv);
    
    gcv.foreground = black;
    gcv.background = _client->GetColorByName("green");
    _gc_change = XCreateGC(GetDisplay(), _id, mask, &gcv);

    gcv.foreground = black;
    gcv.background = _client->GetColorByName("red");
    _gc_ladd = XCreateGC(GetDisplay(), _id, mask, &gcv);

    gcv.foreground = black;
    gcv.background = _client->GetColorByName("blue");
    _gc_radd = XCreateGC(GetDisplay(), _id, mask, &gcv);

    for (int i=0; i<MAXLINES; i++) {
      chars[i] = NULL;
      lntype[i] = 0;
      lnnum[i] = i;
    }
    nlines = top = 0;
    _numbering = 0;
    _marked_start = -1;
    _marked_end = -1;
}

OXDiffFrame::~OXDiffFrame() {
  int i;

  for (i=0; i<nlines; ++i)
    if (chars[i]) delete[] chars[i];
  XFreeGC(GetDisplay(), _gc);
  XFreeGC(GetDisplay(), _gc_change);
  XFreeGC(GetDisplay(), _gc_ladd);
  XFreeGC(GetDisplay(), _gc_radd);
  _client->FreeFont(_font);
}

void OXDiffFrame::SetFont(OXFont *f) {
  if (f) {
    _client->FreeFont(_font);
    _font = f;
    _th = _font->TextHeight();
    XSetFont(GetDisplay(), _gc, _font->GetId());
    XSetFont(GetDisplay(), _gc_change, _font->GetId());
    XSetFont(GetDisplay(), _gc_ladd, _font->GetId());
    XSetFont(GetDisplay(), _gc_radd, _font->GetId());
    XClearArea(GetDisplay(), _id, 0, 0, _w, _h, True);
    //DrawRegion(0, 0, _w, _h);
  }
}

void OXDiffFrame::SetLineNumOn(int on) {
  _numbering = on;
  //XClearWindow(GetDisplay(), _id);
  DrawRegion(0, 0, _w, _h);
}

void OXDiffFrame::Clear() {
  int i;

  for (i=0; i<nlines; ++i)
    if (chars[i]) delete[] chars[i];
  nlines = top = 0;
  XClearWindow(GetDisplay(), _id);
}

int OXDiffFrame::SetLineType(int line, int type) {
  int i = line;
  if (i >= MAXLINES) return 0;

  while (lnnum[i] < line && i < nlines) i++;
  lntype[i-1] = type;

  return 1;
}

int OXDiffFrame::InsLine(int at, char *buf, int type) {
  char line[256], c, *src, *dst;
  int i, cnt = 0;

  if (nlines + 1 >= MAXLINES) return 0;

  src = buf;
  dst = line;
  while (c = *src++) {
    // Expand tabs
      if (c == 0x0D || c == 0x0A)
        break;
      else if (c == 0x09)
        do
          *dst++ = ' ';
        while (((dst-line) & 0x7) && (cnt++ < MAXLINESIZE-1));
      else
        *dst++ = c;
      if (cnt++ >= MAXLINESIZE-1) break;
  }
  *dst = '\0';

  i = at;
  while (i < nlines && lnnum[i] < at) i++;
  if (i != nlines) {
    for (cnt = nlines; cnt >= i; cnt--) {
      chars[cnt+1] = chars[cnt];
      lntype[cnt+1] = lntype[cnt];
      if (type & DISPLAY_DELETE)
        lnnum[cnt+1] = lnnum[cnt];
    }
  }
  chars[i] = new char[strlen(line) + 1];
  strcpy(chars[i], line);
  lntype[i] = type;
  if (type & DISPLAY_DELETE)
    lnnum[i] = -1;
  else
    lnnum[i] = i;

  ++nlines;
}

int OXDiffFrame::AppendLine(char *buf, int type) {
  InsLine(nlines, buf, type);
}

void OXDiffFrame::PrintText() {
  for (int i = 0; i < nlines; i++)
    printf(" %3d | %3d | %s\n", lnnum[i]+1, i+1, chars[i]);
}

int OXDiffFrame::LoadFile(char *filename) {
  FILE *fp;
  char buf[MAXLINESIZE];
  
  if ((fp = fopen(filename, "r")) == NULL) return False;

  if (nlines > 0) Clear();

  // Read each line of the file into the buffer.
  int i = 0;
  while ((fgets(buf, MAXLINESIZE, fp) != NULL) && (i < MAXLINES)) {
    AppendLine(buf, DISPLAY_NORMAL);
    i++;
  }
  fclose(fp);
  SetTopLine(0);
  DrawRegion(0, 0, _w, _h);

  return True;
}

void OXDiffFrame::DrawRegion(int x, int y, int w, int h) {
  int yloc = 0, index = top, i, j;
  XRectangle rect;
  char buf[256], fmt[16], c;
  OFontMetrics fm;
  GC gc;

  _font->GetFontMetrics(&fm);
  
  rect.x = x;
  rect.y = y;
  rect.width = w;
  rect.height = h;

  // Set the clip mask of the GC

  XSetClipRectangles(GetDisplay(), _gc, 0, 0, &rect, 1, Unsorted);

  // Loop through each line until the bottom of the window is reached,
  // or we run out of lines. Redraw any lines that intersect the exposed
  // region.

  int _space = 5, _space2;
  if (_numbering) {
    for (i = 1, j = 10; j < nlines; j *=10, i++) {}
    sprintf(fmt, "%%%dd %%c %%s", i, i);
    sprintf(buf, fmt, nlines, 'C', "");
  } else {
    strcpy(fmt, "%c %s");
    sprintf(buf, fmt, 'C', "");
  }

  _space2 = _font->XTextWidth(buf);

  while (index < nlines && yloc < _h) {
    yloc += _th;
    if ((yloc - _th <= rect.y + rect.height) && (yloc >= rect.y) &&
        index >= 0) {
      memset(buf, ' ', 256);

      if (lntype[index] & DISPLAY_CHANGE)
        gc = _gc_change, c = 'C';
      else if (lntype[index] & DISPLAY_INSERTL) 
        gc = _gc_ladd, c = 'D';
      else if (lntype[index] & DISPLAY_INSERTR)
        gc = _gc_radd, c = 'A';
      else
        gc = _gc, c = ' ';

      if (index >= _marked_start && index <= _marked_end) {
        if (_numbering) {
          sprintf(buf, fmt, lnnum[index]+1, c, chars[index]);
        } else {
          sprintf(buf, fmt, c, chars[index]);
        }
        memset(buf + strlen(buf), ' ', 256 - strlen(buf));
        XDrawImageString(GetDisplay(), _id, gc, _space,
                         yloc - fm.descent, buf, 256);
      } else {
        if (_numbering) {
          sprintf(buf, fmt, lnnum[index]+1, c, "");
        } else {
          sprintf(buf, fmt, c, "");
        }
        XDrawImageString(GetDisplay(), _id, gc, _space,
                         yloc - fm.descent, buf, strlen(buf));

        sprintf(buf, "%s", chars[index]);
        memset(buf + strlen(buf), ' ', 256 - strlen(buf));
        XDrawImageString(GetDisplay(), _id, _gc, _space + _space2,
                         yloc - fm.descent, buf, 256);
      }
    }
    ++index;
  }

  // Set the GC clip mask back to None
  XSetClipMask(GetDisplay(), _gc, None);
}

void OXDiffFrame::CalcMarkRegion(int s, int e, int &rs, int &re) {
  _marked_start = s;
  _marked_end = e;
  while (lnnum[_marked_start] < s && s < nlines) _marked_start++;
  while (lnnum[_marked_end] < e && e < nlines) _marked_end++;
  MarkRegion(_marked_start, _marked_end);
  rs = _marked_start;
  re = _marked_end;
}

void OXDiffFrame::MarkRegion(int s, int e) {
  int page, new_top;
  int visible = GetVisibleLines();

  _marked_start = s;
  _marked_end = e;
  
  // check if s is in the visible space
  page = visible + top;
  if (page > nlines) page = nlines;
  if (s > top && e < page) {
  } else {
    if (e - s >= visible || s < top) {
      SetTopLine(s);
    } else {
      new_top = e - visible + 1;
      SetTopLine(new_top);
    }
  }
  DrawRegion(0, 0, _w, _h);
}

void OXDiffFrame::CenterDiff() {
  if (_marked_start != -1 && _marked_end != -1) {
    int diff = _marked_end - _marked_start;
    int visible = GetVisibleLines();
    if (diff > visible) {
      SetTopLine(_marked_start);
    } else {
      SetTopLine(_marked_start - visible/2 + diff/2);
    }
    DrawRegion(0, 0, _w, _h);
  }
}

void OXDiffFrame::SetTopLine(int new_top) {
  if (top == new_top) return;
  if (new_top < -GetVisibleLines()) new_top = -GetVisibleLines();
  if (new_top > nlines) new_top = nlines;
  ScrollWindow(new_top);
}

void OXDiffFrame::ScrollWindow(int new_top) {
  XPoint points[4];
  int xsrc, ysrc, xdest, ydest;

  // These points are the same for both cases, so set them here.

  points[0].x = points[3].x = 0;
  points[1].x = points[2].x = _w;
  xsrc = xdest = 0;

  if (new_top < top) {
    // scroll down...
    ysrc = 0;
    // convert new_top row position to pixels
    ydest = (top - new_top) * _th;
    // limit the destination to the window height
    if (ydest > _h) ydest = _h;
    // Fill in the points array with the bounding box of the area that
    // needs to be redrawn - that is, the area that is not copied.
    points[1].y = points[0].y = 0;
    points[3].y = points[2].y = ydest + _th; // -1;
  } else {
    // scroll up...
    ydest = 0;
    // convert new_top row position to pixels
    ysrc = (new_top - top) * _th;
    // limit the source to the window height
    if (ysrc > _h) ysrc = _h;
    // Fill in the points array with the bounding box of the area that
    // needs to be redrawn - that is, the area that is not copied.
    points[1].y = points[0].y = _h - ysrc; // +1;
    points[3].y = points[2].y = _h;
  }
  // Set the top line of the text buffer
  top = new_top;
  // Copy the scrolled region to its new position
  XCopyArea(GetDisplay(), _id, _id, _gc, xsrc, ysrc, _w, _h, xdest, ydest);
  // Clear the remaining area of any old text, set the exposure
  // parameter to True to generate expose events for redraw.
  XClearArea(GetDisplay(), _id, points[0].x, points[0].y,
                                0, points[2].y - points[0].y, False/*True*/);

  DrawRegion(points[0].x, points[0].y,
             points[2].x - points[0].x, points[2].y - points[0].y);
}

//----------------------------------------------------------------------

OXDiffView::OXDiffView(OXWindow *parent, int w, int h,
			 unsigned int options, unsigned int back)
		: OXCompositeFrame(parent, w, h, options, back) {
	
    SetLayoutManager(new OHorizontalLayout(this));

    int background = GetResourcePool()->GetDocumentBgndColor();
    SetBackgroundColor(background);

    _textCanvas = new OXDiffFrame(this, 10, 10, CHILD_FRAME, background);
    _vsb = new OXVScrollBar(this, 10, 10, CHILD_FRAME);

    AddFrame(_textCanvas, NULL);
    AddFrame(_vsb, NULL);
    
}

OXDiffView::~OXDiffView() {
}

int OXDiffView::LoadFile(char *fname) {
  int retc = _textCanvas->LoadFile(fname);
  //_vsb->SetRange(_textCanvas->GetLines(), _textCanvas->GetVisibleLines());
  if (retc) {
    _vsb->SetPosition(0);
    Layout();
  }
  return retc;
}

void OXDiffView::Clear() {
  _textCanvas->Clear();
  Layout();
}

void OXDiffView::CenterDiff() {
  _textCanvas->CenterDiff();
  _vsb->SetPosition(_textCanvas->GetTopLine(), False);
}

void OXDiffView::AdjustScrollBar() {
  _vsb->SetPosition(_textCanvas->GetTopLine(), False);
}

int OXDiffView::ProcessMessage(OMessage *msg) {
  OScrollBarMessage *sbmsg;

  switch(msg->type) {
    case MSG_VSCROLL:
      switch(msg->action) {
        case MSG_SLIDERTRACK:
        case MSG_SLIDERPOS:
          sbmsg = (OScrollBarMessage *) msg;
          _textCanvas->SetTopLine(sbmsg->pos);
          break;
      }
      break;
  }
  return True;
}

void OXDiffView::Layout() {
  int lines, vlines, tcw, tch;

  tch = _h - (_bw << 1);
  tcw = _w - (_bw << 1);
  _textCanvas->SetHeight(tch);
  lines = _textCanvas->GetLines();
  vlines = _textCanvas->GetVisibleLines();

  if (lines <= vlines) {
    _vsb->UnmapWindow();
  } else {
    tcw -= _vsb->GetDefaultWidth();
    _vsb->MoveResize(_bw + tcw, _bw, _vsb->GetDefaultWidth(), tch);
    _vsb->MapWindow();
    _vsb->SetRange(lines, vlines);
  }

  _textCanvas->MoveResize(_bw, _bw, tcw, tch);
}

void OXDiffView::DrawBorder() {
  switch (_options & (SUNKEN_FRAME | RAISED_FRAME | DOUBLE_BORDER)) {
  case SUNKEN_FRAME | DOUBLE_BORDER:
    XDrawLine (GetDisplay(), _id, _shadowGC, 0, 0, _w-2, 0);
    XDrawLine (GetDisplay(), _id, _shadowGC, 0, 0, 0, _h-2);
    XDrawLine (GetDisplay(), _id, _blackGC, 1, 1, _w-3, 1);
    XDrawLine (GetDisplay(), _id, _blackGC, 1, 1, 1, _h-3);

    XDrawLine (GetDisplay(), _id, _hilightGC, 0, _h-1, _w-1, _h-1);
    XDrawLine (GetDisplay(), _id, _hilightGC, _w-1, _h-1, _w-1, 0);
    XDrawLine (GetDisplay(), _id, _bckgndGC,  1, _h-2, _w-2, _h-2);
    XDrawLine (GetDisplay(), _id, _bckgndGC,  _w-2, 1, _w-2, _h-2);
    break;

  default:
    OXCompositeFrame::DrawBorder();
    break;
  }
}
