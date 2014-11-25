#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#import "sys/wait.h"

int main( int argc, char* argv[])
{
	int fildes[2];
	pid_t pid;
	int length =0;
	pipe(fildes);
	fdopen(fildes[0],"r");
	fdopen(fildes[1],"w");
	char buf[100];
	char read_buf[100];
	if (!(pid = fork()))
	{
		close(fildes[0]);
		FILE* file = fopen(argv[1],"r");
		while(write(fildes[1], &buf,sizeof(char)*fread(&buf,sizeof(char), 100, file)))
		{
		}
		close(fildes[1]);
		return 0;
	}
	else
	{
		close(fildes[1]);
		int size;
		while((size=read(fildes[0],read_buf,sizeof(char))))
		{
			write(STDOUT_FILENO,read_buf,size);
        }
        close(fildes[0]);
	}
    return 0;
}