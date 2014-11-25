#include <stdio.h>
#include <string.h>
//#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void * start_point(void* a)
{
	printf("======================\n");
	printf("my number is %d\n ", *(int *)a);
	pthread_exit(a);
}
int main(int argc, const char *argv[])
{
	int count = 0;
	int err;
	int n;
	if(argc>0)
		n = atoi(argv[1]);
    while(count<n)
    {
    	pthread_t thread_id; 
    	int *rvalue;//value, returned by the thread
    	err = pthread_create(&thread_id, NULL, start_point,(void*)&count);
    	if(!err)
    	{
    		pthread_join(thread_id, (void**)&rvalue);
    		printf("I was waiting of my child thred.\nIt returns %d\n",
    			*(int*)rvalue);
    		count++;
    	}
    	else printf("There was a problem with creating thread\n");
    }
    return 0;

}