/**************************************************************************

    This is a test program for the OXHelpWindow widget.
    Copyright (C) 2000, Hector Peraza.

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

#include <X11/keysym.h>

#include <xclass/utils.h>
#include <xclass/OXHelpWindow.h>


//----------------------------------------------------------------------

int main(int argc, char **argv) {

  OXClient *clientX = new OXClient;

  char *appname = "";
  if (argc > 1) {
    appname = argv[1];
  } else {
    fprintf(stderr, "usage: %s appname  (for example: %s rx320)\n",
            argv[0], argv[0]);
    return 1;
  }

  int w = 600;  // 450;
  int h = 650;  // 500;

  OXHelpWindow *mainWindow = new OXHelpWindow(clientX->GetRoot(), NULL, w, h,
                                              "index.html", NULL, appname);
  mainWindow->MapWindow();

  clientX->Run();

  return 0;
}
