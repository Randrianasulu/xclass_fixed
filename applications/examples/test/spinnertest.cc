#include <xclass/OXClient.h>
#include <xclass/OXMainFrame.h>
#include <xclass/OXLabel.h>
#include <xclass/OXSpinner.h>

main() {

  OXClient *ClientX = new OXClient();
  OXMainFrame *man = new OXMainFrame(ClientX->GetRoot(), 100, 150);

  // Create first Spinner using defaults
  // 0 - 100 step by 1 roll over & editable

  OXLabel *lab1 = new OXLabel(man, new OString("Editable"));
  OXSpinner *sp = new OXSpinner(man, "test", 100);

  // Create a second Spinner using decimal points
  // also set rollover and editable to false.

  OXLabel *lab2 = new OXLabel(man,new OString("Non-Editable"));
  OXSpinner *sp2 = new OXSpinner(man,"test2", 102);

  sp2->SetEditable(false);
  sp2->SetPrec(3);
  sp2->SetPercent(false);
  sp2->SetRollOver(false);
  sp2->SetRange(0, 9, 0.125);

  sp->Resize(60, sp->GetDefaultHeight());
  sp2->Resize(60, sp2->GetDefaultHeight());

  man->AddFrame(lab1, new OLayoutHints(LHINTS_NORMAL, 5, 5, 1, 1));
  man->AddFrame(sp, new OLayoutHints(LHINTS_NORMAL, 5, 5, 5, 5));

  man->AddFrame(lab2, new OLayoutHints(LHINTS_NORMAL, 5, 5, 1, 1));
  man->AddFrame(sp2, new OLayoutHints(LHINTS_NORMAL, 5, 5, 5, 5));

  man->Resize(man->GetDefaultSize());
  man->MapSubwindows();
  man->MapWindow();

  ClientX->Run();
}
