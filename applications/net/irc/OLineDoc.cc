#include <unistd.h>
#include <ctype.h>

#include "OLineDoc.h"
#include "OXTextView.h"

extern unsigned long IRCcolors[];
extern OGC *_ircGCs[];

//----------------------------------------------------------------------

OLineDoc::OLineDoc() {
  prev = next = this;
  _w = _h = 0;
  lnlen = 0;
}

OLineDoc::~OLineDoc() {
  Clear();
}

OLineDoc *OLineDoc::InsertBefore(OLineDoc *l) {
  l->prev = prev;
  l->next = this;
  prev->next = l;
  prev = l;
  return l;
}

OLineDoc *OLineDoc::InsertAfter(OLineDoc *l) {
  l->prev = this;
  l->next = next;
  next->prev = l;
  next = l;
  return l;
}

void OLineDoc::SetCanvas(OXTextFrame *c) {
  _style_server = c;
  _canvas = c;
  SetColor("white");
}

void OLineDoc::SetColor(char *color) {
  _gc = _style_server->GetGC(color);
}

void OLineDoc::SetColor(int color) {
  _gc = _ircGCs[color];
}

void OLineDoc::Clear() {
  delete[] chars;
  delete[] color;
  delete[] attrib;
  lnlen = 0;
}

int OLineDoc::Fill(char *buf) {
  int  cnt;
  char c, *src;
  char line[MAXLINESIZE], colr[MAXLINESIZE], atrb[MAXLINESIZE];
  int  lh = _gc->_font->ascent + _gc->_font->descent;
  char ca = ATTRIB_NORMAL, cc = 0xFF;

  src = buf;
  cnt = 0;
  while (c = *src++) {
    if (c == 0x09) {
      do {
        line[cnt] = ' ';
        colr[cnt] = cc;
        atrb[cnt] = ca;
        ++cnt;
      } while ((cnt & 0x7) && (cnt < MAXLINESIZE-1));
    } else if (c == 0x02) {  // toggle bold attribute
      if (ca & ATTRIB_BOLD)
        ca &= ~ATTRIB_BOLD;
      else
        ca |= ATTRIB_BOLD;
    } else if (c == 0x16) {  // toggle reverse attribute
      if (ca & ATTRIB_REVERSE)
        ca &= ~ATTRIB_REVERSE;
      else
        ca |= ATTRIB_REVERSE;
    } else if (c == 0x1F) {  // toggle underline attribute
      if (ca & ATTRIB_UNDERLINE)
        ca &= ~ATTRIB_UNDERLINE;
      else
        ca |= ATTRIB_UNDERLINE;
    } else if (c == 0x03) {  // set color or clear color attribute
      if (!isdigit(*src)) {
        ca &= ~(ATTRIB_FGCOLOR | ATTRIB_BGCOLOR);
      } else {
        int fg, bg;
        fg = *src++ - '0';
        if (isdigit(*src)) fg = fg*10 + (*src++ - '0');
        ca |= ATTRIB_FGCOLOR;
        cc &= 0x0F;
        cc |= (char) ((fg << 4) & 0xF0);
        if (*src == ',') {
          ++src;
          if (isdigit(*src)) {
            bg = *src++ - '0';
            if (isdigit(*src)) bg = bg*10 + (*src++ - '0');
            ca |= ATTRIB_BGCOLOR;
            cc &= 0xF0;
            cc |= (char) bg & 0x0F;
          }
        }
      }
    } else if (c > 0x1F) {
      line[cnt] = c;
      colr[cnt] = cc;
      atrb[cnt] = ca;
      ++cnt;
    }
    if (cnt >= MAXLINESIZE-1) break;
  }
  line[cnt] = '\0';
  lnlen = strlen(line);
  chars  = new char[lnlen + 1];
  color  = new char[lnlen + 1];
  attrib = new char[lnlen + 1];
  strcpy(chars, line);
  memcpy(color,  colr, lnlen);
  memcpy(attrib, atrb, lnlen);
  _h = lh * (int)(lnlen / 80) + lh;
  _w = 80;
  return True;
}

int OLineDoc::LoadFile(FILE * file) {
  char buf[MAXLINESIZE];

  if (fgets(buf, MAXLINESIZE, file) != NULL) {
    Fill(buf);
    return True;
  } else
    return False;
}

void OLineDoc::Layout() {
/*
  int charw = XTextWidth(_gc->_font, "H", 1); // 6
  ll = ((_w = _style_server->GetDocWidth()) - (2 * MARGIN)) / charw;
  if (ll == 0) ll = 1;
  _h = _gc->_font->ascent + _gc->_font->descent;
  _h *= (1 + (int)(lnlen / ll));
*/

  char *p = chars, *prev = chars, *chunk = chars;
  int tw, nlines;

  _h = _gc->_font->ascent + _gc->_font->descent;
  nlines = 1;

  int w = (_w = _style_server->GetDocWidth()) - (2 * MARGIN);

  tw = XTextWidth(_gc->_font, chars, lnlen);
  if (tw <= w) return;

  while(1) {
    p = strchr(p, ' ');
    if (p == NULL) {
      tw = XTextWidth(_gc->_font, chunk, strlen(chunk));
      if (tw > w) ++nlines;
      break;
    }
    tw = XTextWidth(_gc->_font, chunk, p-chunk);
    if (tw > w) {
      if (prev == chunk)
        chunk = prev = ++p;
      else
        p = chunk = prev;
      ++nlines;
    } else {
      prev = ++p;
    }
  }

  _h *= nlines;
}

void OLineDoc::DrawRegion(Display *dpy, Drawable d,
                          int x, int y, XRectangle *rect) {
/*
  int i, j, dlen;
  int ld = _gc->_font->ascent;
  int lh = ld + _gc->_font->descent;
  int nrows = lnlen / ll;
  int xl, from;

  for (i = 0; i <= nrows; i++) {
    from = i*ll;
    xl = x + MARGIN;
    for (j = i*ll; j < ((lnlen < ll*(i+1)) ? lnlen : ll*(i+1)); ++j) {
      if ((attrib[j] != attrib[from]) || (color[j] != color[from])) {
        xl += DrawLineSegment(dpy, d, xl, y + i*lh + ld,
                              chars+from, j-from, attrib[from], color[from]);
        from = j;
      }
    }

    if (from != j)
      DrawLineSegment(dpy, d, xl, y + i*lh + ld,
                      chars+from, j-from, attrib[from], color[from]);

  }
*/

  char *p = chars, *prev = chars, *chunk = chars;
  int tw, th;

  int w = (_w = _style_server->GetDocWidth()) - (2 * MARGIN);

  y += _gc->_font->ascent;

  tw = XTextWidth(_gc->_font, chars, lnlen);
  if (tw <= w) {
    DrawLine(dpy, d, x+MARGIN, y, 0, lnlen);
    return;
  }

  th = _gc->_font->ascent + _gc->_font->descent;

  while (1) {
    p = strchr(p, ' ');
    if (p == NULL) {
      tw = XTextWidth(_gc->_font, chunk, strlen(chunk));
      if (tw > w) {
        DrawLine(dpy, d, x+MARGIN, y, chunk-chars, prev-chunk-1);
        y += th;
        DrawLine(dpy, d, x+MARGIN, y, prev-chars, strlen(prev));
      } else {
        DrawLine(dpy, d, x+MARGIN, y, chunk-chars, strlen(chunk));
      }
      break;
    }
    tw = XTextWidth(_gc->_font, chunk, p-chunk);
    if (tw > w) {
      if (prev == chunk)
        prev = ++p;
      else
        p = prev;
      DrawLine(dpy, d, x+MARGIN, y, chunk-chars, prev-chunk-1);
      chunk = prev;
      y += th;
    } else {
      prev = ++p;
    }
  }
/*
  while (1) {
    p = strchr(p, ' ');
    if (p == NULL) {
      if (chunk) DrawLine(dpy, d, x+MARGIN, y, 
                          chunk-chars, strlen(chunk));
      break;
    }
    tw = XTextWidth(_gc->_font, chunk, p-chunk);
    if (tw > w) {
      if (prev == chunk)
        prev = ++p;
      else
        p = prev;
      DrawLine(dpy, d, x+MARGIN, y, chunk-chars, prev-chunk-1);
      chunk = prev;
      y += th;
    } else {
      prev = ++p;
    }
  }
*/
}

void OLineDoc::DrawLine(Display *dpy, Drawable d, int x, int y,
                        int from, int len) {
  int i;
  int xl = x;

  len += from;
  for (i = from; i < len; ++i) {
    if ((attrib[i] != attrib[from]) || (color[i] != color[from])) {
      xl += DrawLineSegment(dpy, d, xl, y,
                            chars+from, i-from, attrib[from], color[from]);
      from = i;
    }
  }

  if (from != i)
    DrawLineSegment(dpy, d, xl, y,
                    chars+from, i-from, attrib[from], color[from]);
}

int OLineDoc::DrawLineSegment(Display *dpy, Drawable d,
                              int x, int y,
                              char *str, int len,
                              char attrib, char color) {
  int tw;
  GC  gc = _gc->_gc;
  unsigned long oldfg = _gc->_foreground,
                oldbg = _gc->_background,
                fg, bg, tmp;

  tw = XTextWidth(_gc->_font, str, len);
  fg = oldfg;
  bg = oldbg;
  if (attrib & ATTRIB_FGCOLOR) fg = IRCcolors[(color>>4) & 0x0F];
  if (attrib & ATTRIB_BGCOLOR) bg = IRCcolors[color & 0x0F];
  if (attrib & ATTRIB_REVERSE) { tmp = fg; fg = bg; bg = tmp; }
  if (oldfg != fg) XSetForeground(dpy, gc, fg);
  if (oldbg != bg) XSetBackground(dpy, gc, bg);
  if (attrib & (ATTRIB_BGCOLOR | ATTRIB_REVERSE))
    XDrawImageString(dpy, d, gc, x, y, str, len);
  else
    XDrawString(dpy, d, gc, x, y, str, len);
  if (attrib & ATTRIB_BOLD)
    XDrawString(dpy, d, gc, x+1, y, str, len);
  if (attrib & ATTRIB_UNDERLINE)
    XDrawLine(dpy, d, gc, x, y, x+tw-1, y);

  if (oldfg != fg) XSetForeground(dpy, gc, oldfg);
  if (oldbg != bg) XSetBackground(dpy, gc, oldbg);

  return tw;
}
