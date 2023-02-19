/* a simple producer/consumer using semaphores and threads
   usage on Solaris:
     gcc thisfile.c -lpthread -lposix4
     a.out numIters
*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define SHARED 1
void *Producer(void *);  /* the two threads */
void *Consumer(void *);
sem_t men, women;    /* the global semaphores */
int data;             /* shared buffer         */
int numIters;
int totalBathroomBreaks = 0;
/* main() -- read command line and create threads, then
             print result when the threads have quit */
int main(int argc, char *argv[]) {
  /* thread ids and attributes */
  pthread_t pid, cid;  
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  numIters = atoi(argv[1]);
  sem_init(&men, SHARED, 1);  /* sem men = 1 */
  sem_init(&women, SHARED, 0);   /* sem women = 0  */
  printf("main started\n");
  pthread_create(&pid, &attr, Producer, NULL);
  pthread_create(&cid, &attr, Consumer, NULL);
  pthread_join(pid, NULL);
  pthread_join(cid, NULL);
  printf("main done\n");
  return 0;
}
/* deposit 1, ..., numIters into the data buffer */
void *Producer(void *arg) {
  int produced;
  printf("Women created\n");
    while(totalBathroomBreaks < 100) {
        printf("Woman is going to work \n");
        sleep(1);
        printf("Woman has done some work and has to poop \n");

        sem_wait(&men);
            printf("Woman has entered the bathroom \n");
            sleep(1);
            printf("*plop*\n");
            sleep(1);
            printf("*flush*\n");
            sleep(1);
            printf("*woosh*\n");
            sleep(1);
            printf("*click*\n");
            totalBathroomBreaks++;
            printf("Woman has finished visit to bathroom \n");
        sem_post(&women);
    }

    

    printf("Total toilet breaks %d\n", totalBathroomBreaks);
  
  return 0;
}
/* fetch numIters items from the buffer and sum them */
void *Consumer(void *arg) {
  int total = 0, consumed;
  printf("Men created\n");

    while(totalBathroomBreaks < 100) {
        printf("Man is going to work \n");
        sleep(1);
        printf("Man has done some work and has to poop \n");

        sem_wait(&women);
            printf("Man has entered the bathroom \n");
            sleep(1);
            printf("*plop*\n");
            sleep(1);
            printf("*flush*\n");
            sleep(1);
            printf("*woosh*\n");
            sleep(1);
            printf("*click*\n");
            totalBathroomBreaks++;
            printf("Man has finished visit to bathroom \n");
        sem_post(&men);
    }

    

    printf("Total toilet breaks %d\n", totalBathroomBreaks);

  return 0;
}