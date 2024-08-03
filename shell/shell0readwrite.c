#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>

#define MAX_BUF 100
char buf[MAX_BUF];

int main(int argc, char **argv) {
    while (1) {
        write(1, "$ ", 2);
        memset(buf, 0, MAX_BUF);
        int numread = read(1, buf, MAX_BUF);
        if (numread == -1)
            return -1;
        else if (numread == 0)
            return 0;
        write(1, "You typed: ", 11);
        write(1, buf, numread);
    }
}
