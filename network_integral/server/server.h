#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/time.h>


#define PENDING_QUEUE_SIZE 20 /*the number of servrers in the queue*/
#define CLIENT_LISTEN_TIMEOUT 2
#define HANDSHAKE_PORT 23239
#define ASSERT(cond) do {\
    if (!(cond)) {\
        printf ("Assert in %s on line %d. errno = %d (%s).\n"\
        , __FILE__, __LINE__, errno, strerror(errno));\
        exit(1);\
    }\
    }while(0)

typedef long long big_num_t;

typedef struct
{
    long double x1;
    long double x2;
} task_t;

typedef struct
{
    long thr_num;
    /*number of threads*/
    int socket;
    task_t task;
    double result;
    int finished;
} worker_t;

//
//const int start_point = 0;
//const int end_point = 1000;
const int BROADCAST = 1232354;
const int PING = 128;
const int THNX = 2323;
const int KILL = 666;
const int STILL_WORKING = 2;
const int DONE = 3;
const int LSTN_PORT = 72165;





