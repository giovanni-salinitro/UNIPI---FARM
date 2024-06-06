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

#include "check_arguments.h"
#include "ConcurrentQueue.h"
#include "Master.h"
#include "Workers.h"
#include "conn.h"
#include "Collector.h"
#include "Result.h"

ResultStructure result = {NULL, 0};

static void maskSignals(sigset_t *set) {
 

    int err;
    struct sigaction s;

    // Creo un set vuoto
    sigemptyset(set);

    memset(&s, 0, sizeof(s));
    s.sa_handler = SIG_IGN;
    err = sigaction(SIGPIPE, &s, NULL);
    if (err == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Aggiungo i segnali specifici che voglio mascherare
    sigaddset(set, SIGINT);
    sigaddset(set, SIGHUP);
    sigaddset(set, SIGTERM);
    sigaddset(set, SIGQUIT);
    sigaddset(set, SIGUSR1);
    

    err = pthread_sigmask(SIG_BLOCK, set, NULL);
    if (err == -1) {
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);
    }

}

void CollectorExec(){

    int socket_fd;
    int n,len;
    long sum;
    char *buffer;
    struct sockaddr_un serv_addr;
    sigset_t signal_set;
    maskSignals(&signal_set);

    SYSCALL_EXIT("socket", socket_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path,SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    SYSCALL_EXIT("connect", notused, connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)), "connect", "");

    while (1) {

            SYSCALL_EXIT("readn", n, readn(socket_fd, &sum, sizeof(long)), "read sum", "");

            if(n!=0){
                 SYSCALL_EXIT("readn", n, readn(socket_fd, &len, sizeof(int)), "read len", "");

                 if(n!=0){
                        buffer=malloc(sizeof(char)*len+1);
                         if (!buffer) {
                              perror("Errore nell'allocazione di memoria per il buffer di lettura");
                                 exit(EXIT_FAILURE);
                        }

                        SYSCALL_EXIT("readn", n, readn(socket_fd, buffer, len), "read buffer", "");
                      
                            if(n!=0){
                                buffer[len] = '\0';

                               if(strcmp(buffer,"print")==0){
                                printf("Collector:Ricevuto segnale di printare i risultati\n");
                                sortElements(&result);
                                PrintResults(&result);
                                fflush(stdout);
                               printf("Collector:Finito di printare i risultati\n\n");
                               }else{

                                         addElement(&result,sum,buffer);
                               }

                               

                                free(buffer);
                            }

                 }
            }
            else{//qui la read mi torna 0 quindi ho fnito

                  printf("Collector is closing\n");
                    break;

            }
    }
  
    sortElements(&result);
    PrintResults(&result);
    freeResultStructure(&result);

    exit(EXIT_SUCCESS);

}