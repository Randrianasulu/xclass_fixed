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

#ifndef __OXDIFF_H
#define __OXDIFF_H

#include "OXDiffView.h"


#define DIFF_LRC      1  // Left Right Change
#define DIFF_RCA      2  // Right Change Add
#define DIFF_LCA      3	 // Left Change Add
#define DIFF_LA       4  // Left Add
#define DIFF_RA       5  // Right Add
#define DIFF_LC       6  // Left Change
#define DIFF_NOTREL   7	 // Undefined


struct ODifference {
  ODifference(char *line);
  int leftStart, leftEnd, rightStart, rightEnd;
  int type;
  char str[20];
  ODifference *next;
};

class OXFont;

class OXDiff : public OXCompositeFrame {
public:
  OXDiff(const OXWindow *p, int w, int h,
         unsigned int options = HORIZONTAL_FRAME);
  ~OXDiff();
    
  int SetLineNumOn(int);
  int SetLeftFile(char * = NULL);
  int SetRightFile(char * = NULL);

  int GetNumDiffs() { return numDiffs; }
  int DoDiff();
  int UnDoDiff();
  int ParseOutput();
  int ShowDiff(int);
  int CenterDiff();
  int CanDoDiff() { return (leftFile && rightFile); }
	
  char *GetDiffStr(int index);
  
  OXFont *GetFont() const { return _bodyLeft->GetTextFrame()->GetFont(); }
  void SetFont(OXFont *f);

private:
  OXDiffView *_bodyLeft, *_bodyRight;
  OXLabel *_leftLabel, *_rightLabel;
  OXVerticalFrame *_vframe1, *_vframe2;
  ODifference *GetDifference(int);

  char *leftFile;
  char *rightFile;
  int numDiffs;
  ODifference *firstDiff;
};


#endif  // __OXDIFF_H
