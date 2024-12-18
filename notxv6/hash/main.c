#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

// The function coletta() has a local char[] coletta.
// put() must make a copy of coletta[] because the stack is reused
void 
coletta()
{
  char coletta[] = "coletta";
  put(coletta, 10);
}

char *ja = "jerri anne\0was here";

int
main(int argc, char *argv[])
{
  // put() must make a copy of opal[] because opal[] is reused
  // and we want two entries - opal and open
  char opal[] = "opal";   // opal[] contains "opal\0"
  put("gusty", 22);
  put(opal, 4);
  opal[2] = 'e';
  opal[3] = 'n';          // opal[] contains "open\0"
  put(opal, 100);
  char *tysug = malloc(6);
  strcpy(tysug, "tysug"); // tysug[] contains "tysug\0"
  // put() must make a copy of tysug[] because tysug[] is reused
  // and we want two entries - tysug and ty
  put(tysug, 222);
  *(tysug+2) = 0;         // tysug[] contains "ty\0ug\0"
  put(tysug, 333);
  coletta();
  put(ja, 20);
  char florence[] = {0x66, 0x6c, 0x6f, 0x72, 0x65, 0x6e, 0x63, 0x65, 0};
  put(florence, 1);
  printf("opal: %d\n", get("opal"));
  printf("tysug: %d\n", get("tysug"));
  printf("notthere: %d\n", get("notthere"));
  if (argc == 3)   // ./hashprog name age has been entered
    put(argv[1], atoi(argv[2]));
  printhash();
}
