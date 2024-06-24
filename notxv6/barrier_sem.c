#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

static int nthread = 1;
static int round = 0;

struct barrier {
  sem_t barrier_turnstile1;  // first turnstile
  sem_t barrier_turnstile2;  // second turnstile
  sem_t barrier_mutex;       // 
  int nthread;               // Number of threads that have reached this round of the barrier
  int round;                 // Barrier round
} bstate;

static void
barrier_init(void)
{
  assert(sem_init(&bstate.barrier_turnstile1, 0, 0) == 0);
  assert(sem_init(&bstate.barrier_turnstile2, 0, 1) == 0);
  assert(sem_init(&bstate.barrier_mutex, 0, 1) == 0);
  bstate.nthread = 0;
}

/* barrier's lines are numbered for lab questions. */
/*  1 */ static void
/*  2 */ barrier()
/*  3 */ {
/*  4 */   sem_wait(&bstate.barrier_mutex);
/*  5 */     bstate.nthread++;
/*  6 */     if(bstate.nthread == nthread){
/*  7 */       sem_wait(&bstate.barrier_turnstile2);
/*  8 */       sem_post(&bstate.barrier_turnstile1); // wakeup one thread
/*  9 */     }
/* 10 */   sem_post(&bstate.barrier_mutex);
/* 11 */ 
/* 12 */   sem_wait(&bstate.barrier_turnstile1); // Wait until nthreads block
/* 13 */   sem_post(&bstate.barrier_turnstile1); // Wakeup next thread
/* 14 */ 
/* 15 */   // Critical point
/* 16 */ 
/* 17 */   sem_wait(&bstate.barrier_mutex);
/* 18 */     bstate.nthread--;
/* 19 */     if(bstate.nthread == 0){
/* 20 */       sem_wait(&bstate.barrier_turnstile1);
/* 21 */       sem_post(&bstate.barrier_turnstile2);
/* 22 */       bstate.round++;
/* 23 */     }
/* 24 */   sem_post(&bstate.barrier_mutex);
/* 25 */ 
/* 26 */   sem_wait(&bstate.barrier_turnstile2);
/* 27 */   sem_post(&bstate.barrier_turnstile2);
/* 28 */ }

static void *
thread(void *xa)
{
  long n = (long) xa;
  long delay;
  int i;

  for (i = 0; i < 20000; i++) {
    int t = bstate.round;
    assert (i == t);
    barrier();
    usleep(random() % 100);
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  long i;
  double t1, t0;

  if (argc < 2) {
    fprintf(stderr, "%s: %s nthread\n", argv[0], argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);

  barrier_init();

  for(i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void *) i) == 0);
  }
  for(i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0);
  }
  printf("OK; passed\n");
}
