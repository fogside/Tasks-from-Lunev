#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#import "sys/wait.h"

int main(int argc, char *argv[])
{
	int n, count =0, status;
	scanf("%d",&n);
	pid_t pid;
	while(count<n)
	{
		if(!(pid=fork()))
		{
			printf("number:%d, pid: %d, ppid: %d \n", count, getpid(), getppid());
			return 0;
		}
		else
		{
			count++;
			wait(&status);
		}
	}
	return 0;
}