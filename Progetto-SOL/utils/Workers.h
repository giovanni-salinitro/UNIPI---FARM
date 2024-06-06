#ifndef WORKERS
#define WORKERS

#include "ConcurrentQueue.h"

typedef struct ThreadArgs {
    ConcurrentQueue *coda1;
    ConcurrentQueue *coda2;
    struct timespec *delay;
    int fd;
} ThreadArgs;


void *MasterThread(void *arg);
void *Worker(void *arg);


#endif