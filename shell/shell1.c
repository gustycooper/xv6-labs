#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_BUF 100
#define MAX_WORDS 10
char buf[MAX_BUF];                  // buffer for line
char whitespace[] = " \t\r\n\v";    // defn of whitespace
char *words_on_line[MAX_WORDS];     // collects words on the line

int main(int argc, char **argv) {
    while (1) {
        fprintf(stdout, "gusty $ ");
        fflush(stdout);  
        memset(buf, 0, MAX_BUF);
        if (!(fgets(buf, MAX_BUF, stdin)))
            return -1;
        buf[strcspn(buf, "\n")] = '\0';
        char *s = buf, *end_buf = buf + strlen(buf);
        int numwords = 0;
        while (1) {
            while (s < end_buf && strchr(whitespace, *s)) s++;  // skip whitespace
            if (*s == 0)                                        // eol - done
                break;
            words_on_line[numwords++] = s;                      // address of word
            while (s < end_buf && !strchr(whitespace, *s)) s++; // consume non whitespace
            *s = 0;                                             // null terminate word
        }
        words_on_line[numwords] = 0;                            // null terminate words_on_line
        if (numwords == 0)
            continue;
        if (fork() == 0) {
            execvp(words_on_line[0], words_on_line);
            fprintf(stdout, "cmd not found: %s\n", words_on_line[0]);
            exit(1);
        }
        wait(NULL);
    }
}
