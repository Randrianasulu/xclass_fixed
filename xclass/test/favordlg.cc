#include <xclass/OXClient.h>
#include <xclass/OXFileDialog.h>

char *filetypes[] = {
  "C/C++ Files",      "*.c|*.cc|*.h",
  "JavaScript Files", "*.js",
  "All files",        "*",
  0,		      0
};

int main() {
  OFileInfo fi;
  OXClient *client;

  client = new OXClient();
  
  fi.file_types = filetypes;
 
  new OXFileDialog(client->GetRoot(), 0, FDLG_OPEN | FDLG_FAVOURITES, &fi);

  if (fi.filename)
    printf("File Selected is %s/%s\n", fi.ini_dir, fi.filename);

  new OXFileDialog(client->GetRoot(), 0, FDLG_OPEN, &fi);

  if (fi.filename)
    printf("File Selected is %s/%s\n", fi.ini_dir, fi.filename);
}
