#include <xclass/OXClient.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OString.h>
#include <xclass/OXComboBox.h>

main() {

  OXClient *ClientX = new OXClient();

  OXMainFrame *man = new OXMainFrame(ClientX->GetRoot(), 300, 100);

  OXComboBox *cb = new OXComboBox(man, "test", 100);

  man->AddFrame(cb, new OLayoutHints(LHINTS_CENTER_X | LHINTS_CENTER_Y));
  cb->AddEntry(new OString("Testing 1"), 1);
  cb->AddEntry(new OString("Testing 2"), 2);
  cb->AddEntry(new OString("Testing 3"), 3);
  cb->AddEntry(new OString("Testing 4"), 4);
  cb->AddEntry(new OString("Testing 5"), 5);
  cb->AddEntry(new OString("Testing 6"), 6);
  cb->AddEntry(new OString("Testing 7"), 7);
  cb->AddEntry(new OString("Testing 8"), 8);

  man->MapSubwindows();
  man->MapWindow();
  man->Layout();

  ClientX->Run();
}
