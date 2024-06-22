#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int rseedv = 0;

int
main(int argc, char *argv[])
{
  if(argc != 2 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s seed\n", argv[0]);
    exit(1);
  }

  rseed(atoi(argv[1]));

  int i;
  for (i = 0; i < 10; i++)
    printf("%d\n", rinter(100));

  exit(0);
}
