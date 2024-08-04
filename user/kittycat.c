#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

void
kittycat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "kittycat: write error\n");
      exit(1);
    }
  }
  if(n < 0){
    fprintf(2, "kittycat: read error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int fd;

  if(argc != 1){
    fprintf(2, "kittycat: No args allowed\n");
    exit(1);
  }

  if((fd = open("kittycat.txt", 0)) < 0){
    fprintf(2, "kittycat: cannot open kittycat.txt\n");
    exit(1);
  }
  kittycat(fd);
  close(fd);
  exit(0);
}
