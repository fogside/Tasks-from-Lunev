//
// Queue
//
// Created by Евгения Ческидова on 20/02/15.
// Copyright (c) 2015 FOGSIDE. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"

typedef struct Nod
{
    void *next;
    int data;
} Nod;

struct Queue
{
    Nod *tail;
    Nod *head;
};

struct Iterator
{
    Nod *current;
    Nod *head;
};

Queue *CreateMyQueue()
{
    Queue *queue = malloc(sizeof(Queue));
    if (queue == NULL)
    {
        perror("Memory allocating mistake");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

int GetHead(Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return -1;

    }
    if (queue->head == NULL)
    {
        perror("There's now head");
        return -1;
    }
    return queue->head->data;
}

int GetTail(Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return -1;

    }
    if (queue->tail == NULL)
    {
        perror("There's no tail");
        return -1;
    }
    return queue->tail->data;
}


int Enqueue(int data, Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return -1;

    }
    Nod *new_nod = malloc(sizeof(Nod));
    new_nod->data = data;
    new_nod->next = NULL;
    if ((queue->head) == NULL)
    {
        queue->head = new_nod;
        queue->tail = new_nod;
    }
    else
    {
        queue->tail->next = new_nod;
        queue->tail = new_nod;
    }
    return 0;
}

int Dequeue(Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return -1;

    }
    if (queue->head == NULL)
    {
        perror("There're no elements left to extract");
        return -1;

    }
    int data = queue->head->data;
    Nod *tmp = queue->head->next;
    free(queue->head);
    queue->head = tmp;
    return data;
}


void DeleteQueue(Queue *queue)
{
    if(queue == NULL)
    {
        perror("Bad queue pointer");
        return;

    }
    Nod *tmp;
    while (queue->head != NULL)
    {
        tmp = queue->head;
        queue->head = queue->head->next;
        free(tmp);
    }
    free(queue);
}

int Size(Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return -1;

    }
    Iterator *_iter = CreateIterator(queue);
    int count = 0;
    while (IsThereNext(_iter))
    {
        count++;
        GetNext(_iter);
    }
    DeleteIterator(_iter);
    return count;
}

int isEmpty(Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return -1;

    }
    return (queue->head == NULL) ? 1 : 0;
}


/**-----------ITERATOR------------**/



Iterator *CreateIterator(Queue *queue)
{
    if(queue==NULL)
    {
        perror("Bad queue pointer");
        return NULL;

    }
    if(queue->head == NULL)
    {
        perror("Empty queue was given");
        return NULL;
    }
    Iterator *iter = malloc(sizeof(Iterator));
    iter->head = queue->head;
    iter->current = queue->head;
    return iter;
}

int IsThereNext(Iterator *iter)
{
    if(iter==NULL)
    {
        perror("Bad iter pointer");
        return -1;

    }
    if(iter->current->next == NULL)
        return 0;
    return 1;
}


int GetCurrent(Iterator *iter)
{
    if(iter==NULL)
    {
        perror("Bad iter pointer");
        return -1;
    }
    return iter->current->data;
}


int GetNext(Iterator *iter)
{
    if(iter==NULL)
    {
        perror("Bad iter pointer");
        return -1;
    }
    assert(IsThereNext(iter));
    iter->current = iter->current->next;
    return iter->current->data;
}

void DeleteIterator(Iterator *iter)
{
    if(iter==NULL)
    {
        perror("Bad *iter pointer");
        return;
    }
    free(iter);
}

int RestartIter(Iterator *iter)
{
    if(iter==NULL)
    {
        perror("Bad iter pointer");
        return -1;
    }
    iter->current = iter->head;
    return 0;
}
