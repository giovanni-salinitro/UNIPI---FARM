
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h> 
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <util.h>
#include <time.h>
#include <ConcurrentQueue.h>
#include <Workers.h>
#include <conn.h>




extern volatile sig_atomic_t Exit;
extern int qsize;// Capacità massima di coda2

extern pthread_mutex_t mutex2;
extern ThreadArgs args;
volatile sig_atomic_t finito=0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_worker = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_master = PTHREAD_COND_INITIALIZER;

void CalcoloBinario(Node *node){

    int len,n;
      FILE *file = fopen(node->filename, "rb");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return ;
    }

    long longSum = 0;
    long num;
    int counter = 0;

    while (fread(&num, sizeof(long), 1, file) == 1) {
        long product = num * counter;
        longSum += product;
        counter++;
    }

    len=strlen(node->filename);

    LOCK(&mutex2);

    SYSCALL_EXIT("writen", n, writen(node->fd, &longSum, sizeof(long)), "write sum", "");
    SYSCALL_EXIT("writen", n, writen(node->fd, &len, sizeof(int)), "write len", "");
    SYSCALL_EXIT("writen", n, writen(node->fd, node->filename, len), "write filename", "");

    UNLOCK(&mutex2);

    fclose(file);

    return;

}

void *MasterThread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    ConcurrentQueue *coda1 = args->coda1;
    ConcurrentQueue *coda2 = args->coda2;
    struct timespec *delay = args->delay;
    int fd=args->fd;
    struct timespec remaining;
   
    while (Exit) {
         
        Node *node = dequeue(coda1);
        if (node == NULL) {
            break;
        }

       
       LOCK(&mutex);
        while (coda2->size >= qsize) {
           
          WAIT(&cond_master, &mutex);
        }

        enqueue(coda2, node->filename,fd);


       SIGNAL(&cond_worker);
        UNLOCK(&mutex);

        
        freeNode(node);

       
         if (clock_nanosleep(CLOCK_MONOTONIC, 0, delay, &remaining) != 0) {
            perror("Errore nella clock_nanosleep\n");
            return ((void*)EXIT_FAILURE);
        }
      
    

    }

    finito=1;
    BCAST(&cond_worker);
    pthread_exit(0);
}

void *Worker(void *arg) {
ConcurrentQueue *coda2 = (ConcurrentQueue *)arg;

    while (1) {
        Node *node;

        LOCK(&mutex);

        if(!finito){
            
            while (coda2->size <=0){
                
                if(finito){
                     break;
                 }
             
             WAIT(&cond_worker,&mutex);
            }

        } else if(finito){
            
            if (coda2->size <= 0) {
                 UNLOCK(&mutex);
                 break;
            }
        }

        node = dequeue(coda2);

       SIGNAL(&cond_master);
       UNLOCK(&mutex);

        if (node != NULL) {
          CalcoloBinario(node);
           freeNode(node);
        }
    }
    
    pthread_exit(0);
}