#ifndef CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_H



typedef struct Node {
    int fd;
    char *filename;
    struct Node *next;
} Node;

typedef struct ConcurrentQueue {
    Node *head;
    int size;
} ConcurrentQueue;


void initQueue(ConcurrentQueue *queue);
void enqueue(ConcurrentQueue *queue, const char *filename,int fd);
Node *dequeue(ConcurrentQueue *queue);
void freeNode(Node *node);
void freeQueue(ConcurrentQueue* q);
void queueDisplay(ConcurrentQueue* q);



#endif 
