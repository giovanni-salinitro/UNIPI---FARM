#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ConcurrentQueue.h>

//Funzione per inizializzare la coda
void initQueue(ConcurrentQueue *queue) {
    queue->head = NULL;
    queue->size = 0;
}

//Funzione per inserire un elemento in coda
void enqueue(ConcurrentQueue *queue, const char *filename,int fd) {
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        perror("Errore nell'allocazione di memoria per il nodo");
        exit(EXIT_FAILURE);
    }
    
    int len = strlen(filename);
    newNode->filename = malloc(len + 1);
    if (!newNode->filename) {
        free(newNode);
        perror("Errore nell'allocazione di memoria per il filename");
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->filename, filename);
    newNode->fd=fd;
    newNode->next = NULL;

    if (queue->head == NULL) {
        queue->head = newNode;
    } else {
        Node *currentNode = queue->head;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
        currentNode->next = newNode;
    }

    queue->size++;

}

//Funzione per togliere un elemento dalla coda
Node *dequeue(ConcurrentQueue *queue) {
    if (queue->size <= 0) {
        return NULL;
    }

    Node *node = queue->head;
    queue->head = queue->head->next;
    queue->size--;

    return node;
}

//Funzione per liberare dalla memoria un elemento dalla coda
void freeNode(Node *node) {
    if (node != NULL) {
        free(node->filename);
        free(node);
    }
}

//Funzione per liberare dalla memoria una coda
void freeQueue(ConcurrentQueue* q){
  
    while((q)->head!=NULL){
        Node *delete = (q)->head;
        (q)->head = delete->next;
        free(delete->filename);
        free(delete);
    }

    free(q);
    
}

//Funzione per stampare gli elementi di una coda
void queueDisplay(ConcurrentQueue* q){
    Node *curr = q->head;
    while(curr != NULL){
        printf("%s ",curr->filename);
        if(curr->next != NULL)
            printf("-> ");

        curr = curr->next;
    }
    printf("\n");


}

