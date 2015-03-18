#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/*
Queue *CreateMyQueue();

int Enqueue(int data, Queue *queue);

int GetHead(Queue *queue);

int GetTail(Queue *queue);

int Dequeue(Queue **queue);

int Size(Queue *queue);

int isEmpty(Queue *queue);

void DeleteQueue(Queue *queue);*/


/**--------------- ITERATOR --------------------**/
/*
Iterator *CreateIterator(Queue *queue);

int GetNext(Iterator *iter);

int GetCurrent(Iterator *iter);

int IsThereNext(Iterator *iter);

int RestartIter(Iterator *iter);

void DeleteIterator(Iterator *iter);
*/

int main()
{
    /***QUEUE TEST***/

    printf("\n/***QUEUE TEST***/\n");

    Queue *queue = CreateMyQueue();
    for (int i = 0; i <= 10; i++)
        Enqueue(i, queue);
    printf("GETHEAD %d\n", GetHead(queue));
    printf("GETTAIL %d\n", GetTail(queue));

    for (int i = 0; i <= 5; i++)
        printf("Dequeue %d\n", Dequeue(queue));

    printf("GETHEAD %d\n", GetHead(queue));
    printf("isEmpty? %d\n", isEmpty(queue));

    printf("SIZE %d\n", Size(queue));
    for (int i = 0; i < 5; i++)
        printf("Dequeue %d\n", Dequeue(queue));
    printf("isEmpty? %d\n\n", isEmpty(queue));

    printf("DeleteQueue\n");
    DeleteQueue(&queue);

    printf("Enqueue\n");


    /*** ITERATOR TEST ***/

    Queue *queue1 = CreateMyQueue();
    for (int i = 0; i <= 10; i++)
        Enqueue(i, queue1);
    printf("\n/*** ITERATOR TEST ***/\n");
    printf("\nLets find the max\n");

    Iterator *iter = CreateIterator(queue1);
    int max = GetNext(iter);
    while (IsThereNext(iter))
    {
        int tmp = GetNext(iter);
        if (tmp > max)
            max = tmp;
    }


    printf("MAX = %d\n", max);
    // printf("GETNEXT: %d\n", GetNext(iter));
    printf("RestartIter\n");
    RestartIter(iter);
    printf("And after restart GETNEXT1: %d\n", GetNext(iter));
    printf("And after restart GETNEXT2: %d\n", GetNext(iter));
    printf("And after restart GETNEXT3: %d\n", GetNext(iter));
    printf("And after restart GETNEXT4: %d\n", GetNext(iter));
    printf("SO, GetCurrent: %d\n", GetCurrent(iter));

    DeleteQueue(&queue1);
    printf("And after delete queue GETNEXT: %d\n", GetNext(iter));

    // RestartIter(iter);
    // printf("And after restart GETNEXT1: %d\n", GetNext(iter));



    printf("So.....\n\n");
    printf("DELETE ITER!\n");
    DeleteIterator(&iter);

    //To execute the exception with the empty queue
    Queue *queue2 = CreateMyQueue();
    Iterator *iter1 = CreateIterator(queue2);
    //And what is going with GetHead and GetTail?
    GetHead(queue2);
    GetTail(queue2);
    //And with an empty queue and Dequeue?
    Dequeue(queue2);

    /**FINAL TEST**/
Enqueue(6, NULL);
GetHead(NULL);
GetTail(NULL);
Dequeue(NULL);
Size(NULL);
isEmpty(NULL);
DeleteQueue(NULL);


/**--------------- ITERATOR --------------------**/
CreateIterator(NULL);
GetNext(NULL);
GetCurrent(NULL);
IsThereNext(NULL);
RestartIter(NULL);
DeleteIterator(NULL);

return 0;

}