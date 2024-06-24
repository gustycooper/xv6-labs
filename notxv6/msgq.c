#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "msgq.h"

struct msgq *msgq_init(int num_msgs) {
    // Place your code here
    return mq;
}

/*
 * Send msg on msgq *mq
 * malloc()s memory for struct msgs.
 * malloc() for char *msg in msgs is done via strdup().
 */
int msgq_send(struct msgq *mq, char *msg) {
    // Place your code here
    return 1;
}

/*
 * Return first msg in msgq *mq, blocks until a msg is in msgq
 * returns mq->msgs, which was strdup()ed in msgq_send
 */
char *msgq_recv(struct msgq *mq) {
    // Place your code here
    return m;
}

/*
 * returns the msg_count of mq.
 * Due to thread scheduling, msg_count may be wrong by the time you get it.
 */
int msgq_len(struct msgq *mq) {
    return mq->msg_count;
}

void msgq_show(struct msgq *mq) {
    // Place your code here
}
