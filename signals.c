//
//  main.c
//  signals
//
//  Created by Евгения Ческидова on 18.12.14.
//  Copyright (c) 2014 fogside. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>



pid_t child_pid;
char receive_char = 0;
int global_offset = 128;

void InitProcMask()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);
}

void InitSigAction(void (*handler)(int), int signo)
{
    struct sigaction sigaction_struct;
    sigaction_struct.sa_flags = 0;
    sigaction_struct.sa_handler = handler;
    sigfillset(&sigaction_struct.sa_mask);
    sigaction(signo, &sigaction_struct, NULL);
}

void Child_exit(int signo)
{
//    printf("%s", __FUNCTION__);
//    fflush(stdout);
    exit(EXIT_SUCCESS);
}

// If parent get SIGCHLD
void Parent_exit(int signo)
{
//    printf("%s", __FUNCTION__);
//    fflush(stdout);
    exit(EXIT_SUCCESS);
}

// Handler for SIGUSR1 in Child proc
void Empty(int signo)
{
//    printf("%s", __FUNCTION__);
//    fflush(stdout);
}

// SIGUSR1
void Get_one_handler(int signo)
{
//    printf("%s", __FUNCTION__);
//    fflush(stdout);

    receive_char |= global_offset;
    global_offset = global_offset >> 1;
    kill(child_pid, SIGUSR1);
}

// SIGUSR2
void Get_zero_handler(int signo)
{
//    printf("%s", __FUNCTION__);
//    fflush(stdout);

    global_offset = global_offset >> 1;
    kill(child_pid, SIGUSR1);
}


void Child(char *path)
{
//    printf("%s started\n", __FUNCTION__);
//    fflush(stdout);

    InitSigAction(Child_exit, SIGALRM);
    InitSigAction(Empty, SIGUSR1);

//    printf("%s initialized\n", __FUNCTION__);
//    fflush(stdout);

    sigset_t set;
    sigemptyset(&set);

    pid_t ppid = getppid();
    int fd = open(path, O_RDONLY);
    char c;
    int offset;
    while (read(fd, &c, 1) > 0)
    {
        offset = 128;
        while (offset > 0)
        {
            alarm(1);
//            printf("%s sent bit", __FUNCTION__);
//            fflush(stdout);
            /* we should die if our parent
             will not be able to send signal
             for a second*/

            if (offset & c)
                kill(ppid, SIGUSR1);
            else
                kill(ppid, SIGUSR2);
            offset = offset >> 1;

            /* wait a signal from parent */
            sigsuspend(&set);
        }
    }
    exit(1);
}


void Parent()
{
    // we should die if SIGCHLD
    InitSigAction(Parent_exit, SIGCHLD);

    // Get SIGUSR1 == get 1
    InitSigAction(Get_one_handler, SIGUSR1);

    // Get SIGUSR2 == get 2
    InitSigAction(Get_zero_handler, SIGUSR2);

    sigset_t set;
    sigemptyset(&set);

    while(1)
    {
        if(!global_offset)
        {
            write(STDOUT_FILENO, &receive_char, 1);
            fflush(stdout);
            global_offset = 128;
            receive_char = 0;
        }
        sigsuspend(&set);
    }
}


int main(int argc, char *argv[])
{
    /* Block the signals: SIGUSR1, SIGUSR2, SIGCHLD */
    InitProcMask();

    child_pid = fork();

    if (child_pid)
        Parent();
    else
        Child(argv[1]);
    return 0;
}
