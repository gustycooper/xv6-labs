#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define MAX_BUF 100
#define MAX_WORDS 10
char buf[MAX_BUF];                                         // buffer for line
char whitespace[] = " \t\r\n\v";                           // defn of whitespace
char *words_on_line[MAX_WORDS];                            // collects words on the line
int p[2];                                                  // used for pipe ends

void sigint() {                                            // signal handler for CTL-C (SIGINT)
    fprintf(stdout, "\ngusty $ ");
    fflush(stdout);
}

void childexit() {                                         // signal handler for SIGCHLD
    int wstat;
    while (1) {
        int pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
        if (pid == 0 || pid == -1)
            return;
    }
}

int hasstring(int numwords, char *str) {                    // see if str in in words_on_line
    for (int i = 0; i < numwords; i++)
        if (strcmp(words_on_line[i], str) == 0) {
            words_on_line[i] = 0;
            return i;                                       // return pos if str is in words_on_line
        }
    return 0;                                               // return 0 if str is not in words_on_line
}

int hasredirect(int numwords) {                           // see if < or > is in words_on_line
    int mode = 0, found = 0, i;
    for (i = 0; i < numwords; i++)
        if (strcmp(words_on_line[i], ">") == 0 || strcmp(words_on_line[i], "<") == 0) {
            mode = strcmp(words_on_line[i], "<") == 0 ? O_RDONLY : O_WRONLY|O_CREAT;
            words_on_line[i] = 0;
            found = 1;
            break;
        }
    return found ? 0x0f000000 | (i << 16) | mode : 0;      // return pos of < in bits 16-23, mode in lower 16 bits
}

void executecmd(int pos) {                                  // execute cmd in words_on_lin[pos]
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

void pipedo(int inout, int wordpos) {
    dup2(p[inout], inout);                                  // dup std<in/out> onto pipe end
    close(p[0]); close(p[1]);                               // close pipe ends
    executecmd(wordpos);                                    // execute command in words_on_line[wordpos]
}

int cdcmd() {
    if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {  // Clumsy 
        if (chdir(buf+3))
            fprintf(stderr, "cannot cd %s\n", buf+3);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    signal (SIGCHLD, childexit);
    signal (SIGINT, sigint);
    while (1) {
        int chid, numwords, pipepos, redirect, backgroundpos;
        fprintf(stdout, "gusty $ ");                        // create prompt
        fflush(stdout);  
        memset(buf, 0, MAX_BUF);
        if (!(fgets(buf, MAX_BUF, stdin)))                  // exit shell on CTL-D
            return -1;
        buf[strcspn(buf, "\n")] = '\0';                     // fgets return string with \n, get rid of \n
        if (cdcmd())                                        // if user enters cd, continue loop
            continue;
        if ((numwords = getwords()) == 0)          // put buf into words_on_line[]
            continue;                                       // continue loop on blank line
        if ((pipepos = hasstring(numwords, "|"))) {         // if line has a pipe, process pipe
            pipe(p);
            if (fork() == 0)
                pipedo(1, 0);                               // execute words_on_line[0] to stdout of pipe p[]
            if (fork() == 0)
                pipedo(0, pipepos+1);                       // execute words_on_line[pipepos+1] to stdin of pipe p[]
            close(p[0]); close(p[1]);                       // parent closes pipe ends
            wait(NULL); wait(NULL);                         // parent waits for the two children
            continue;
        }
        (backgroundpos = hasstring(numwords, "&")) ? numwords-- : 0; // see if background cmd. if so, adjust numwords
        if ((chid = fork()) == 0) {
            if ((redirect = hasredirect(numwords))) {       // see if redirect cmd
                if ((redirect & 0xffff) == O_RDONLY)        // if redirect is <, close stdin, mode is read only
                    close(0);
                else
                    close(1);                               // if redirect is >, close stdout, mode is write
                if (open(words_on_line[((redirect >> 16) & 0xff)+1], redirect & 0xffff, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
                    fprintf(stderr, "open %s failed\n", words_on_line[(redirect>>16)+1]);
                    return -1;
                }
            }
            executecmd(0);                                  // execute words_on_line[0]
        }
        if (!backgroundpos)                                 // if not background, wait for child
            waitpid(chid, NULL, 0);
    }
}
