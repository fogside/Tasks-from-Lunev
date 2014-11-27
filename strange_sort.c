#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <stdio.h>
#include <unistd.h>
#define MAGIC_NUMBER 23

struct msg_buf
{
    long mtype;
    int array_element;
};


void my_sort(int *arr, int arr_size, int i_from, int i_to)
{
    int i, j;
    long int temp = 0;


    if (i_from < 0) i_from = 0;
    if (i_to >= arr_size) i_to = arr_size - 1;
    for (i = i_from; i < i_to; i++)
    {
        for (j = i + 1; j <= i_to; j++)
        {
            if (arr[j] < arr[i])
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int n = argc - 1;//size of the array
    int *array1 = (int*) malloc(sizeof(int)*n);//part 1
    int *array2=(int*) malloc(sizeof(int)*n);//part 2
    int *array=(int*) malloc(sizeof(int)*n);
    for (int i = 0; i < n; i++)
    {
        array[i] = atoi(argv[i+1]);
    }

    //DIVIDE IT INTO TWO PARTS:
    int sizeOfAr1, sizeOfAr2, i = sizeOfAr1 = sizeOfAr2 = 0;
    int middle_elt = array[n / 2];
    while (i < n)
    {
        if (array[i] < middle_elt)
        {
            array1[sizeOfAr1] = array[i];
            sizeOfAr1++;
        }
        else
        {
            array2[sizeOfAr2] = array[i];
            sizeOfAr2++;
        }
        i++;
    }
    struct msg_buf myMsg;
    myMsg.mtype = 0;
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT|0666);

    //STARTING TO PARALLELIZE
    int frk = fork();
    if (!frk) // CHILD
    {
        my_sort(array1, sizeOfAr1, 0, sizeOfAr1);
        myMsg.mtype = MAGIC_NUMBER;
        for(int i = 0; i<sizeOfAr1;i++)
        {
            myMsg.array_element = array1[i];
            msgsnd(msqid, &myMsg, sizeof(int), 0);
        }
        return 0;
    }
    else
    {
        my_sort(array2, sizeOfAr2, 0, sizeOfAr2);
        for(int j =0; j<sizeOfAr1;j++)
        {
            msgrcv(msqid, &myMsg, sizeof(int), MAGIC_NUMBER, 0);
            printf("%d ",myMsg.array_element);
        }
        for(int j =0; j<sizeOfAr2;j++)
        {
            printf("%d ",array2[j]);
        }
        printf("\n");
    }
    return 0;
}
