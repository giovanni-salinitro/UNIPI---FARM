
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>


#include "check_arguments.h"
#include "Master.h"
#include "conn.h"
#include "util.h"
#include "Collector.h"


void cleanup() {
    unlink(SOCKNAME);
}

int main(int argc, char *argv[]){

 cleanup();    
 atexit(cleanup);  

 int socket_fd;

 SYSCALL_EXIT("socket", socket_fd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");

 struct sockaddr_un serv_addr;

  memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
  strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);


  int notused;
    SYSCALL_EXIT("bind", notused, bind(socket_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "bind", "");
    SYSCALL_EXIT("listen", notused, listen(socket_fd, MAXBACKLOG), "listen", "");

pid_t pid = fork();
    if (pid < 0)
        perror("Errore nella fork");

if(pid!=0){//il parent è il Master
    MasterExec(argc,argv,socket_fd);
}
else{//il child è il Collector

   CollectorExec();
}

 

 return 0;

}