#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "msgq.h"

// SEE Labs/GdbLldbLab for more information on lldb - lowlevel debugger

struct msgq *mq;
char *strings1[100];
char *strings2[100];
char *strings3[100];

//
// Main threads
//
char *messages[] = { "msg1", "msg2", "hellomsg", "gustymsg" };

// sends msgs in messages[]
void *promtAndSend(void *arg) {
    for (int i = 0; i < sizeof(messages)/sizeof(char*); i++) {
        char response[80];
        printf("Send? ");
        scanf("%s", response);
        if (response[0] == 'y' || response[0] == 'Y') {
            printf("sending: %s\n", messages[i]);
            msgq_send(mq, messages[i]);
        }
    }
    return NULL;
}

// consume messges in msgq
void *recvMsgs(void *arg) {
    sleep(5);
    int msg_count = msgq_len(mq);
    printf("mq msg_count: %d\n", msg_count);
    for (int i = 0; i < msg_count; i++) {
        char *m = msgq_recv(mq);
        printf("recvMsgs: %s\n", m);
        //free(m);
    }
    return NULL;
}

void *passiton(void *arg) {
    long lme = (long) arg;
    int me = lme;
    while (1) {
        sleep(1);
        printf("passiton%d initial msgq_len: %d\n", me, msgq_len(mq));
        char *m = msgq_recv(mq);
        printf("passiton%d: %p %p %s\n", me, &m, m, m);
        printf("passiton%d after recv msgq_len: %d\n", me, msgq_len(mq));
        msgq_send(mq, m);
        printf("passiton%d after send msgq_len: %d\n", me, msgq_len(mq));
        free(m);
    }
    return NULL;
}

void* sendmax() {
    printf("sending %s\n", "Message 1");
    msgq_send(mq, "Message 1");
    printf("sending %s\n", "Message 2");
    msgq_send(mq, "Message 2");
    printf("sending %s\n", "Message 3");
    msgq_send(mq, "Message 3");
    printf("sending %s\n", "Message 4");
    msgq_send(mq, "Message 3");
    printf("sending %s\n", "Message 5 with queue of 4 - blocks");
    msgq_send(mq, "Message 5 with queue of 4");    //this should block
    sleep(10);
    printf("sent %s\n", "Message 5 with queue of 4");
    return NULL;
}

void* recvmax() {
    for (int i=0; i<5; i++) {
        char* msg = msgq_recv(mq);
        printf("received %s\n", msg);
    }
    return NULL;
}

void* producer(void* idv) {
    long lid = (long)idv;
    int id = lid;

    for (int i=0; i<50; i++) {
        char msg[30];
        snprintf(msg, 30, "msg tid:%d, mnum:%d", id, i);
        msgq_send(mq, msg);
    }

    printf("%d finished sending\n", id);
    return NULL;
}

void* consumer(void* idv) {
    long lid = (long)idv;
    int id = lid;
    int i=0;

    while(1) {
        char* msg = msgq_recv(mq);
        if (msg != NULL) {
            if (id == 1) {
                strings1[i] = strdup(msg);
            } else if (id == 2) {
                strings2[i] = strdup(msg);
            } else if (id == 3) {
                strings3[i] = strdup(msg);
            }
            i++;
            free(msg);
        }
    }
}

#define MSGQLEN 4

int main(int argc, char *argv[]) {
    pthread_t p1, p2, p3, p4, p5;
    mq = msgq_init(MSGQLEN);
    char test = '1';
    if (argc == 2)
        test = argv[1][0];
    switch (test) {
      case '1':
        printf("test fill and empty msgq\n");
        pthread_create(&p1, NULL, promtAndSend, NULL);
        pthread_join(p1, NULL);
        printf("msgq_show() after filling for test 1:\n");
        msgq_show(mq);
        pthread_create(&p2, NULL, recvMsgs, NULL);
        pthread_join(p2, NULL);
        printf("msgq_show() after all consumed by test 1:\n");
        msgq_show(mq);
        break;
      case '2':
        printf("test fill msgs and pass it on\n");
        pthread_create(&p1, NULL, promtAndSend, NULL);
        pthread_join(p1, NULL);
        printf("msgq_show() after filling for test 2:\n");
        msgq_show(mq);
        pthread_create(&p1, NULL, passiton, (void *)1);
        pthread_create(&p2, NULL, passiton, (void *)2);
        pthread_join(p1, NULL);
        pthread_join(p2, NULL);
        break;
      case '3':
        printf("test send blocks on full msgq\n");
        printf("final message will block and be sent last\n");
        pthread_create(&p1, NULL, sendmax, NULL);
        sleep(10);
        pthread_create(&p2, NULL, recvmax, NULL);
        pthread_join(p1, NULL);
        pthread_join(p2, NULL);
        break;
      case '4':
        printf("test recv blocks on empty msgq\n");
        printf("Starting recv...\n");
        pthread_create(&p2, NULL, recvmax, NULL);
        printf("waiting 10 seconds for msgs\n");
        sleep(10);
        pthread_create(&p1, NULL, sendmax, NULL);
        pthread_join(p1, NULL);
        pthread_join(p2, NULL);
        break;
      case '5':
        printf("test 2 producers 3 consumers\n");
		pthread_create(&p1, NULL, consumer, (void*)1);
		pthread_create(&p2, NULL, consumer, (void*)2);
		pthread_create(&p3, NULL, consumer, (void*)3);
		pthread_create(&p4, NULL, producer, (void*)4);
		pthread_create(&p5, NULL, producer, (void*)5);

        pthread_join(p4, NULL);
        pthread_join(p5, NULL);
        printf("wait for consumers to finish\n");
        sleep(5);
        //print all 100
        printf("consumer 1\n");
        for (int i=0; i<100 && strings1[i] != NULL; i++)
            printf("\t%s\n", strings1[i]);
        printf("consumer 2\n");
        for (int i=0; i<100 && strings2[i] != NULL; i++)
            printf("\t%s\n", strings2[i]);
        printf("consumer 3\n");
        for (int i=0; i<100 && strings3[i] != NULL; i++)
            printf("\t%s\n", strings3[i]);
        break;
      default:
        printf("invalid test selection!\n");
        break;
    }
    return 0;
}

