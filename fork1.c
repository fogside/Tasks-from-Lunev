//
//  main.c
//  Trying_to_survive
//
//  Created by Евгения Ческидова on 18.09.14.
//  Copyright (c) 2014 fogside. All rights reserved.
//

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#import "sys/wait.h"

int main(int argc, char *argv[])
{
    pid_t pid;
    //int n = (int) argv[1];
    int n;
    int status;
    scanf("%d",&n);
    int n1=n+1;
    pid = fork();
    while (n>0)
    {
        if (pid > 0)
        {
            break;
        }
        else
        {
            printf(">>>I'm the %d process\n "
                    "my id: %d \n"
                    " parent id: %d\n", n1-n, getpid(), getppid());
            n--;
            pid = fork();
        }
    }

    wait(&status);
    return 0;
}
