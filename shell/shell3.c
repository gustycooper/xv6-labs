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
int p[2];                           // used for pipe ends

int haspipe(int n) {
    for (int i = 0; i < n; i++)
        if (strcmp(words_on_line[i], "|") == 0) {
            words_on_line[i] = 0;
            return i;
        }
    return 0;
}

void executecmd(int pos) {
    execvp(words_on_line[pos], words_on_line+pos);
    fprintf(stdout, "cmd not found: %s\n", words_on_line[0]);
    exit(1);
}

int getwords() {
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
    return numwords;
}

void pipedo(int pipe, int wordpos) {
    dup2(p[pipe], pipe);
    close(p[0]); close(p[1]);
    executecmd(wordpos);
}

int main(int argc, char **argv) {
    while (1) {
        int numwords, pipepos;
        fprintf(stdout, "gusty $ ");
        fflush(stdout);  
        memset(buf, 0, MAX_BUF);
        if (!(fgets(buf, MAX_BUF, stdin)))
            return -1;
        buf[strcspn(buf, "\n")] = '\0';
        if ((numwords = getwords()) == 0)
            continue;
        if (numwords == 0)
            continue;
        if ((pipepos = haspipe(numwords))) {
            pipe(p);
            if (fork() == 0)
                pipedo(1, 0);
            if (fork() == 0)
                pipedo(0, pipepos+1);
            close(p[0]); close(p[1]);
            wait(NULL); wait(NULL);
            continue;
        }
        if (fork() == 0)
            executecmd(0);
        wait(NULL);
    }
}
