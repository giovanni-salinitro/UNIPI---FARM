#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 

#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <util.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <signal.h>
#include <conn.h>


#include "check_arguments.h"
#include "ConcurrentQueue.h"
#include "Master.h"
#include "Workers.h"



ConcurrentQueue coda1;
ConcurrentQueue coda2;

int nthreads=NTHREADS;
 int qsize=QUEUESIZE;
 long delay=DELAY;

pthread_t Master; //MasterThread
pthread_mutex_t mutex2=PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t Exit = 1;
extern volatile sig_atomic_t finito;


struct timespec ts;
ThreadArgs args;

 int coll_fd;
 char *sigusr1="print";

 

void *signal_thread(void *arg) {
    int sig;
    sigset_t signal_set;

   
    sigemptyset(&signal_set);

    sigaddset(&signal_set, SIGINT);
    sigaddset(&signal_set, SIGUSR1);
    sigaddset(&signal_set, SIGTERM);
    sigaddset(&signal_set, SIGHUP);
    sigaddset(&signal_set, SIGQUIT);
    sigaddset(&signal_set, SIGPIPE);
    sigaddset(&signal_set, SIGALRM);

    while (1) {
       
        if (sigwait(&signal_set, &sig) != 0) {
             perror("errore nella sigwait\n");
             exit(EXIT_FAILURE);
         }

        
         if (sig == SIGUSR1) {
          
           printf("Master:Ricevuto segnale SIGUSR1\n");
             int n;
             long longSum = 1;
             int len=strlen(sigusr1);
              LOCK(&mutex2);

              SYSCALL_EXIT("writen", n, writen(coll_fd, &longSum, sizeof(long)), "write sum", "");
              SYSCALL_EXIT("writen", n, writen(coll_fd, &len, sizeof(int)), "write len", "");
              SYSCALL_EXIT("writen", n, writen(coll_fd, sigusr1, len), "write print command", "");

             UNLOCK(&mutex2);
        }else if(sig == SIGALRM){

                //qui master e worker hanno finito,il processo Master può terminare,l'handler riceve SIGALRM e termina
                 break;
        }else if(sig == SIGPIPE){
                 printf("Master:Ricevuto segnale SIGPIPE\n");
                //termino Master
                Exit=0;
                //termino workers
                finito=1;

        }
        else {
            printf("Master:Ricevuto segnale di chiusura\n");
              Exit=0;
        }
    }
   
    pthread_exit(0);
}



void MasterExec(int argc,char* argv[],int socket_fd){

 int res;

 int err;

 res=checkarguments(argc,argv,&nthreads,&qsize,&delay,&coda1);
 if(res!=0){
    fprintf(stderr,"Errore lettura argomenti\n");
 }
 else{

     sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGHUP);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGPIPE);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGALRM);

   

     err = pthread_sigmask(SIG_BLOCK, &set, NULL);
      if (err == -1) {
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);
       }


    SYSCALL_EXIT("accept", coll_fd, accept(socket_fd, (struct sockaddr*)NULL ,NULL), "accept", "");

    //setto il delay
    ts.tv_sec = delay / 1000;
    ts.tv_nsec = (delay % 1000) * 1000000;
    args.coda1 = &coda1;
    args.coda2 = &coda2;
    args.delay = &ts;
    args.fd=coll_fd;
    
    pthread_t Workers[nthreads]; // Numero di worker concorrenti desiderati
    pthread_t Signal_handler;

      if (pthread_create(&Signal_handler, NULL, signal_thread, NULL) != 0) {
        fprintf(stderr, "pthread_create failed (SignalThread)\n");
         return ((void)EXIT_FAILURE);
     } 

   if (pthread_create(&Master, NULL, MasterThread, &args) != 0) {
        fprintf(stderr, "pthread_create failed (MasterThread)\n");
         return ((void)EXIT_FAILURE);
     }    

  
    for (int i = 0; i < nthreads; i++) {
       
        if (pthread_create(&Workers[i], NULL, Worker, &coda2) != 0) {
        fprintf(stderr, "pthread_create failed (Worker %d)\n",i);
         return ((void)EXIT_FAILURE);
         }    
    }


     if (pthread_join(Master, NULL) != 0) {
        fprintf(stderr, "pthread_join failed (MasterThread)\n");
         return ((void)EXIT_FAILURE);
    }

    for (int i = 0; i < nthreads; i++) {
      
        if (pthread_join(Workers[i], NULL) != 0) {
             fprintf(stderr, "pthread_join failed (Worker %d)\n",i);
              return ((void)EXIT_FAILURE);
         }
    }

     if(pthread_kill(Signal_handler,SIGALRM)!=0){
         perror("errore nella kill del signal handler\n");
         exit(EXIT_FAILURE);
     }

    if (pthread_join(Signal_handler, NULL) != 0) {
        fprintf(stderr, "pthread_join failed (SignalThread)\n");
         return ((void)EXIT_FAILURE);
    }

    close(coll_fd);

  }
  
  exit(EXIT_SUCCESS);
}


