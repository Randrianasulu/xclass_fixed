#include <stdio.h>

#include <xclass/OXClient.h>
#include <xclass/OXMainFrame.h>

main() {
  OXClient *clientX = new OXClient();

  OXMainFrame *main1 = new OXMainFrame(clientX->GetRoot(), 100, 100);
  main1->MapWindow();
  main1->SetWindowName("1");

  OXMainFrame *main2 = new OXMainFrame(clientX->GetRoot(), 100, 100);
  main2->MapWindow();
  main2->SetWindowName("2");

  clientX->Run();
  printf("OXClient returned (1,2)\n");

  OXMainFrame *main3 = new OXMainFrame(clientX->GetRoot(), 100, 100);
  main3->MapWindow();
  main3->SetWindowName("3");

  clientX->Run();
  printf("OXClient returned (3)\n");

  OXMainFrame *main4 = new OXMainFrame(clientX->GetRoot(), 100, 100);
  main4->MapWindow();
  main4->SetWindowName("4");

  clientX->Run();
  printf("OXClient returned (4)\n");

  return 0;
}
