#include <stdio.h>
#include "TDList.h"
#include <string.h>
#include <xclass/OString.h>

/*

gcc -o modeparsertest -g modeparsertest.cc -lxclass -lXpm -lX11

this is a work in progress to create a working parser for channel modes.
this works using either argv[1] as the mode to parse or the built in
sample..

+ooblk mike Big_Mac *!*@*.lamer.org 20 mykey 
+siK-l+o-o+bk Big_Mac mike *!*@*.lamer.org mykey 
+stinKo-o+bkl thomas mike *!*@lamer.org mykey 100

*/

typedef TDDLList<OString *> stringList;

char *str = "+smtinKlo-o+bk 10 Big_Mac mike *!*@*.lamer.org mykey";

stringList *ParseMode(const char *line) {

  stringList *sl = new stringList();
  char *ptr, *cp,*cp2, *ptr2;

  cp = strdup(line);

  //printf("cp is :%s\n",cp);
  ptr = strtok(cp," ");
  while (ptr) {
    if (ptr) sl->Add(new OString(ptr));
    ptr = strtok(0," ");
/*    if (cp) sl->Add(new OString(cp));
    ptr2 = ptr++;
//  *ptr2 = 0;
    printf("ptr in while:%s\n", ptr);
    printf("cp is in while:%s\n", cp);
    cp = ptr;
    ptr = strchr(cp, ' ');
    printf("ptr end of while:%s\n", ptr);
    printf("cp end of while:%s\n", cp);
 */
  }

  delete[] cp;
  return sl;
}

void ProcessModeChange(stringList *s) {
  const char*mode_str = s->GetAt(1)->GetString();
  int i, bit, par;
  bool add = true;
  par = 2;
  
  unsigned long mode_bits = 0L;

  if (!mode_str) return;

  for (i=0; i<strlen(mode_str); ++i) {
    bit = mode_str[i];
    if (bit =='+')
      add = true;
    else if (bit == '-')
      add = false;
    else {
    switch (bit) {
      case 'i':
        printf("%s Invite\n",add ? "Adding" : "Removing");
        break;
      case 't':
        printf("%s Topic\n",add ? "Adding" : "Removing");
        break;
      case 'm':
        printf("%s Moderated\n",add ? "Adding" : "Removing");
        break;
      case 'n':
        printf("%s No Msg\n",add ? "Adding" : "Removing");
        break;
      case 's':
        printf("%s Secret\n",add ? "Adding" : "Removing");
        break;
      case 'p':
        printf("%s Private\n",add ? "Adding" : "Removing");
        break;
      case 'K':
        printf("%s Knock\n",add ? "Adding" : "Removing");
        break;
      case 'o':
        printf("%s op %s\n",add ? "Adding" : "Removing",s->GetAt(par++)->GetString());
        break;
      case 'v':
        printf("%s voiced %s\n",add ? "Adding" : "Removing",s->GetAt(par++)->GetString());
        break;
      case 'l':
        printf("%s Limit %s\n",add ? "Adding" : "Removing",
               add ? s->GetAt(par++)->GetString() : "");
        break;
      case 'b':
        printf("%s Ban %s\n",add ? "Adding" : "Removing",s->GetAt(par++)->GetString());
        break;
      case 'k':
        printf("%s Key %s\n",add ? "Adding" : "Removing",s->GetAt(par++)->GetString());
        break;

      default:
        break;
      }    
    }
  }
}

main(int argc,char *argv[]) {
  stringList *sl;

  if (argc > 1) {
    printf("Mode is %s\n-------------------------------\n", argv[1]);
    sl = ParseMode(argv[1]);
  } else {
    printf("Mode is %s\n-------------------------------\n", str);
    sl = ParseMode(str);
  }
//  for (int i = 1 ; i < sl->GetSize()+1; i++)
//    printf("Word #%d:%s\n", i, sl->GetAt(i)->GetString());
  ProcessModeChange(sl);
  delete sl;
}
