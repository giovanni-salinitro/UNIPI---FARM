#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include "check_arguments.h"
#include <dirent.h>
#include <sys/stat.h>

#include "ConcurrentQueue.h"
#include "util.h"

#define MAXFILENAME 255

//Funzione che mi controlla se la stringa passata(filename) è un file
//ritorna true se lo è, false altrimenti
bool isFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

//Funzione che mi dice se il file è regolare
//ritorna true se lo è, false altrimenti
bool isRegularFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

//Funzione che mi dice se il file è binario
//ritorna true se lo è, false altrimenti
bool isBinaryFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        int ch;
        while ((ch = fgetc(file)) != EOF) {
            if (ch < 0x09 || (ch > 0x0D && ch < 0x20) || ch == 0x7F) {
                fclose(file);
                return true;
            }
        }
        fclose(file);
        return false;
    }
    return false;
}


//funzione che prende una directory e una coda, la naviga e aggiunge il pathname dei file binari nella coda 
void navigateDirectory(const char *dirname, ConcurrentQueue *queue) {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char path[MAXFILENAME];
    int result;
   

    dir = opendir(dirname);
    if (!dir) {
        perror("Errore nell'apertura della directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

      
        SYSCALL_EXIT("snprintf",result,snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name),"snpritf error","");
      

        if (lstat(path, &fileStat) < 0) {
            perror("Errore nel recupero delle informazioni del file");
            continue;
        }

        if (S_ISDIR(fileStat.st_mode)) {
            // Se l'elemento è una directory, navigo ricorsivamente
            navigateDirectory(path,queue);
        } else {
            // Se l'elemento è un file binario e regolare, lo metto nella coda1
            if(isRegularFile(path) && isBinaryFile(path))
            enqueue(queue,path,-1);
          
        }
    }

    closedir(dir);
}







//Funzione per verificare gli argomenti da linea di comando
//ritorna 0 se tutto ok, -1 altrimenti 
int checkarguments(int argc, char* argv[],int *n,int *q,long *t,ConcurrentQueue *queue){
    int num,qsize;
    long delay;
    int opt;
    char *p;
     long tmp,tmp1,tmp2;
    

    if(argc<2){
        printf("Troppi pochi argomenti passati\n Uso: %s -[Opzioni] -[Directory]\n",argv[0]);
        return -1;
    }

           
        
        
        while((opt = getopt(argc, argv, ":n:q:t:d:")) != -1){
            switch (opt)
            {
            case 'n' :
               
                if(isNumber(optarg,&tmp)!=0){
                    printf("l'argomento -n non è valido\n");
                    return -1;
                }else{
                    num=atoi(optarg);
                    if(num==0){
                        printf("l'argomento di -n non può essere 0");
                        return -1;
                    }else{
                        *n=num;
                    }
                }
                break;

            case 'q':
            
                if(isNumber(optarg,&tmp1)!=0){
                     printf("l'argomento -q non è valido\n");
                    return -1;
                }else{
                    qsize=atoi(optarg);
                    if(qsize==0){
                        printf("l'argomento di -q non può essere 0");
                        return -1;
                    }else{
                        *q=qsize;
                    }
                }
                break;

            case 't' :
                
                if(isNumber(optarg,&tmp2)!=0){
                     printf("l'argomento -t non è valido\n");
                    return -1;
                }else{
                delay = strtol(optarg,&p,10);
                *t=delay;}
                break;

            case 'd':
                  
                    navigateDirectory(optarg,queue);
                    break;

            case '?': 
                    if(optopt == '-'){
                        fprintf(stderr,"Opzione sconosciuta: %c\n", optopt);
                        return -1;
                    }
                    break;
            
            default:;
               
            }
        }


//argomennti non opzionali
    for (int i = 1; i < argc; i++)
    {
      
        if(isRegularFile(argv[i]) && isBinaryFile(argv[i])){
            enqueue(queue,argv[i],-1);
        }


    }  
    return 0;

}
