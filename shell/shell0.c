#include <stdio.h>
#include <string.h>

#define MAX_BUF 100
char buf[MAX_BUF];

int main(int argc, char **argv) {
    while (1) {
        printf("$ ");
        memset(buf, 0, MAX_BUF);
        if (!(fgets(buf, MAX_BUF, stdin)))
            return -1;
        printf("You typed: %s\n", buf);
    }
}
