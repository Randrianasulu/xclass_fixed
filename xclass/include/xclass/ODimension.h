/**************************************************************************

    This file is part of Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Hector Peraza.

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

#ifndef __ODIMENSION_H
#define __ODIMENSION_H


//--------------------------------------------------------------------

class ODimension {
public:
  // attributes
  unsigned int w, h;

  // Constructors
  ODimension() {}
  ODimension(const unsigned int width, const unsigned int height)
               { w = width; h = height; }
  ODimension(const ODimension &d)
               { w = d.w; h = d.h; }

  bool operator == (const ODimension &b) const 
               { return ((w == b.w) && (h == b.h)); }
  ODimension operator - (const ODimension &b) const
               { return ODimension(w - b.w, h - b.h); }
  ODimension operator + (const ODimension &b) const
               { return ODimension(w + b.w, h + b.h); }
};


class OPosition {
public:
  // attributes
  int x, y;

  // Constructors
  OPosition() {}
  OPosition(const int xc, const int yc)
               { x = xc; y = yc; }
  OPosition(const OPosition &p)
               { x = p.x; y = p.y; }

  bool operator == (const OPosition &b) const
               { return ((x == b.x) && (y == b.y)); }
  OPosition operator - (const OPosition &b) const
               { return OPosition(x - b.x, y - b.y); }
  OPosition operator + (const OPosition &b) const
               { return OPosition(x + b.x, y + b.y); }
};


#endif  // __ODIMENSION_H
