/**************************************************************************

    This file is part of xclass.
    Copyright (C) 1996-2000 Hector Peraza.

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

#ifndef __ORECTANGLE_H
#define __ORECTANGLE_H

#include <xclass/utils.h>
#include <xclass/ODimension.h>


//--------------------------------------------------------------------

class ORectangle {
public:
  // attributes
  int x, y;
  unsigned int w, h;

  // constructors
  ORectangle() { empty(); }
  ORectangle(int rx, int ry, unsigned int rw, unsigned int rh)
               { x = rx; y = ry; w = rw; h = rh; }
  ORectangle(const OPosition &p, const ODimension &d)
               { x = p.x; y = p.y; w = d.w; h = d.h; }
  ORectangle(const ORectangle &r)
               { x = r.x; y = r.y; w = r.w; h = r.h; }

  // methods
  bool contains(int px, int py) const
               { return ((px >= x) && (px < x+w) &&
                         (py >= y) && (py < y+h)); }
  bool contains(OPosition &p) const
               { return ((p.x >= x) && (p.x < x+w) &&
                         (p.y >= y) && (p.y < y+h)); }
  bool contains(ORectangle &r) const
               { return (w * h); }
  bool intersects(ORectangle &r) const
               { return ((x <= r.x + r.w - 1) && (x + w - 1 >= r.x) &&
                         (y <= r.y + r.h - 1) && (y + h - 1 >= r.y)); }
  int area() const
               { return (w * h); }
  ODimension size() const
               { return ODimension(w, h); }
  OPosition left_top() const
               { return OPosition(x, y); }
  OPosition right_bottom() const
               { return OPosition(x + w - 1, y + h - 1); }
  void merge(ORectangle &r)
               { int max_x = max(x + w, r.x + r.w); x = min(x, r.x);
                 int max_y = max(y + h, r.y + r.h); y = min(y, r.y);
                 w = max_x - x;
                 h = max_y - y; }
  void empty() { x = y = 0; w = h = 0; }
  bool is_empty() const { return ((w == 0) && (h == 0)); }
};


#endif  // __ORECTANGLE_H
