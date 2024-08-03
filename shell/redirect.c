#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    close(0);
    open(argv[1], O_RDONLY, 0644);
    close(1);
    open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0644);
    char line[100];
    fgets(line, 100, stdin);
    printf("%s\n", line);
}
