#include <stdio.h>
#include <string.h>

#include "libLameNet.h"

int main() {
  LameNetStart();

  char *taddr = LameListen("23421\0");

  printf("Address created, index is %s\n", taddr);

  while (1) {
    char *recdata;

    recdata = LameServRead(taddr);

    if (strcmp(recdata, "") != 0) {
      printf("data: %s", recdata);
    }
  }

  LameNetStop();
  return 0;
}
