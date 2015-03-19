#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct Queue Queue;
typedef struct Iterator Iterator;

Queue *CreateMyQueue();

int Enqueue(int data, Queue *queue);

int GetHead(Queue *queue);

int GetTail(Queue *queue);

int Dequeue(Queue *queue);

void DeleteQueue(Queue *queue);

int Size(Queue *queue);

int isEmpty(Queue *queue);

/**--------------- ITERATOR --------------------**/

Iterator *CreateIterator(Queue *queue);

int GetNext(Iterator *iter);

int GetCurrent(Iterator *iter);

int IsThereNext(Iterator *iter);

int RestartIter(Iterator *iter);

void DeleteIterator(Iterator *iter);


#endif