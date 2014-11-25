#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#import "sys/wait.h"
#include <sys/errno.h>
#include <string.h>
#define SIZE_OF_MSG 128

struct msg_buf
{
    long mtype;
    char mtext[SIZE_OF_MSG];
};



int main(int argc, const char* argv[])
{
    int n = atoi(argv[1]);
    int count = 1;
    int count1 = 1;
    int msqid;
    msqid = msgget(IPC_PRIVATE, IPC_CREAT|0666);

//creating processes
    pid_t frk;
    struct msg_buf mymsg;

    while(count<=n)
    {
        if(!(frk=fork())) // Rebonok
        {
            break;
        }
        else
        {
            count++;
        }
    }
    if(frk)
    	count1 = n;

    while(count1>0)
    {
        if(!frk)//child
        {
            mymsg.mtype = count;

            msgrcv(msqid, &mymsg, SIZE_OF_MSG, count,0);
            printf("%d %s \n",count, mymsg.mtext);
            fflush(stdout);
            mymsg.mtype = count+n;
            msgsnd(msqid, &mymsg, sizeof(mymsg.mtext), 0);
            return 0;
        }
        else
        {
            sprintf(mymsg.mtext,"I'm %d",count1);
            mymsg.mtype = count1;
            msgsnd(msqid, &mymsg, sizeof(mymsg.mtext), 0);
            msgrcv(msqid, &mymsg, SIZE_OF_MSG, count1+n,0);

            count1--;
        }
    }
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}