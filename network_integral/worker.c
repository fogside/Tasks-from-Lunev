//
//  worker
//
//  Created by Евгения Ческидова on 03/07/15.
//  Copyright (c) 2015 ___FOGSIDE___. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#ifdef __MACH__

#include <mach/clock.h>
#include <mach/mach.h>

#endif

#include "server.h"


struct sockaddr_in listen_to_broadcast(int broadcast_socket);

int broadcast_listening_socket();

int listening_socket();

int send_number_of_threads(int socket, int threads);

int connect_to_client(int socket_tcp, struct sockaddr_in addr_client_p);

int receive_task(int socket, my_task_t *task);

double evaluate(my_task_t *task, int thr_num, int socket);

typedef struct {
    int socket;
    long double *sum;
} sock_and_sum_t;

int main(int argc, const char *argv[]) {

    if(argc < 2)
    {   
        printf("Введите количество тред, пожалуйста!\n");
        return 1;
    }

    int thr_num = strtol(argv[1], NULL, 10);
    ASSERT(thr_num > 0);

    // create broadcast socket //

    int broadcast_socket = broadcast_listening_socket();

    fflush(NULL);
    for (; ;) {
        // getting sender data //

        struct sockaddr_in sender = listen_to_broadcast(broadcast_socket);

        // creating tcp listening socket //

        int tcp_socket = listening_socket();

        // making connection //

        if (connect_to_client(tcp_socket, sender)) {
            /* if failed then... */
            close(tcp_socket);
            printf("failed connect to client");
            continue;
        }
        fflush(NULL);

        if (send_number_of_threads(tcp_socket, thr_num)) {
            /* if failed then... */
            close(tcp_socket);
            printf("failed send thread info\n");
            continue;
        }

        my_task_t task;
        if (receive_task(tcp_socket, &task)) {
            /* if failed then... */
            close(tcp_socket);
            printf("failed receive task!\n");
            continue;
        }
        fflush(NULL);

        ASSERT(!evaluate(&task, thr_num, tcp_socket));
        printf("MISSION COMPLITED\n");
        break;
    }

    return 0;
}


int receive_task(int socket, my_task_t *task) {
    printf("trying to receive task....\n");
    ssize_t size = recv(socket, task, sizeof(my_task_t), 0);
    if (size != sizeof(my_task_t))
        return 1;
    printf("receive task: x1 = %Lf, x2 = %Lf\n", task->x1, task->x2);
    fflush(NULL);
    return 0;
}


struct sockaddr_in listen_to_broadcast(int broadcast_socket) {
    int msg = 0;
    struct sockaddr_in sender;
    socklen_t size = sizeof(sender);
    printf("started to listen to broadcast...\n");
    fflush(NULL);
    for (; ;) {
        recvfrom(broadcast_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &sender, &size);
        if (msg == BROADCAST) {
            printf("broadcast received\n");
            fflush(NULL);
            sender.sin_port = htons(LSTN_PORT);
            break;
        }
    }
    return sender;
}

int send_number_of_threads(int sock, int threads) {
    printf("sending number of threads....\n");
    if (send(sock, &threads, sizeof(threads), 0) == sizeof(threads)) {
        return 0;
    }
    return 1;
}

int broadcast_listening_socket() {
    int socket_listen = socket(PF_INET, SOCK_DGRAM, 0);

    int enable_val = 1;

    /* SO_REUSEADDR indicates that the rules used in validating addresses sup-
     plied in a bind(2) call should allow reuse of local addresses.

     SO_REUSEPORT allows completely duplicate bindings by multiple processes
     if they all set SO_REUSEPORT before binding the port.  This option per-
     mits multiple instances of a program to each receive UDP/IP multicast or
     broadcast datagrams destined for the bound port. */

    setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &enable_val, sizeof(enable_val));
    setsockopt(socket_listen, SOL_SOCKET, SO_REUSEPORT, &enable_val, sizeof(enable_val));

    struct sockaddr_in addr_listen = {.sin_family = AF_INET, .sin_port = htons(HANDSHAKE_PORT), .sin_addr.s_addr = htonl(INADDR_ANY)};
    bind(socket_listen, (struct sockaddr *) &addr_listen, sizeof(addr_listen));

    return socket_listen;
}

int listening_socket() {
    int socket_tcp = socket(PF_INET, SOCK_STREAM, 0);

//    struct timeval tv = {.tv_sec = 2};
//    setsockopt(socket_tcp, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof(struct timeval));
//    setsockopt(socket_tcp, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval));

    int enable_val = 1;
    setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEADDR, &enable_val, sizeof(enable_val));
    setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEPORT, &enable_val, sizeof(enable_val));

    return socket_tcp;
}


int connect_to_client(int socket_tcp, struct sockaddr_in addr_client_p) {
    /* connect socket_tcp with client */
    printf("connecting socket_tcp with client... \n");
    int connect_res = connect(socket_tcp, (struct sockaddr const *) &addr_client_p, sizeof(struct sockaddr_in));
    if (!connect_res) {
        return 0;
    }
    return 1;
}


void *thread_func(void *task) {
    // printf("here %d\n", (int)count);
    long double start_point = ((my_task_t *) task)->x1;
    long double end_point = ((my_task_t *) task)->x2;
    double dx = 0.0000001;
    long double counter = start_point + dx;
    long double sum = 0;

    while (counter <= end_point) {
        //if()
        sum += counter * dx;
        counter += dx;
    }

    ((my_task_t *) task)->x1 = sum;
    printf("my x1: %Lf, x2: %Lf, counter: %Lf, my sum: %Lf\n", start_point, end_point, counter, sum);
    fflush(NULL);
    return NULL;
}

void *ping_thread(void *sock_sum) {
    printf("come into ping_thread...\n");
    fflush(NULL);
    int sock = (*(sock_and_sum_t *) sock_sum).socket;
    ssize_t size;
    const int STILL_WORKING = 1;
    const int DONE = 2;
    int buf;
    for (; ;) {
        size = recv(sock, &buf, sizeof(int), 0);
        printf("Receive buf = %d\n", buf);
        fflush(NULL);
        if (size != sizeof(int)) {
            printf("In ping_thread received failed\n");
            continue;
        }
        if (buf == KILL) {
            printf("GOT KILL(!)\n");
            (*(sock_and_sum_t *) sock_sum).socket = -1;
            return (void *) 1;
        }
        if (buf != PING) {
            printf("error: in ping buf != PING\n");
            continue;
        }
        /* if we get ping and we are currently calculating... */
        if ((*(sock_and_sum_t *) sock_sum).sum == NULL) {
            printf("Send STILL_WORKING..\n");
            fflush(NULL);
            size = send(sock, &STILL_WORKING, sizeof(int), 0);
            if (size != sizeof(int)) {
                printf("In ping_thread send response failed\n");
                continue;
            }
        }
        else {
            printf("Work done!\n");
            size = send(sock, &DONE, sizeof(int), 0);
            if (size != sizeof(int)) {
                printf("In ping send DONE has failed\n");
            }
            printf("DONE calculating\n");

            long double result = *((*(sock_and_sum_t *) sock_sum).sum);
            printf("Result is %Lf\n", result);
            fflush(NULL);

            //####################

            size = send(sock, &result, sizeof(long double), 0);

            if (size != sizeof(long double)) {
                printf("In ping send result has failed\n");
            }
            size = recv(sock, &buf, sizeof(int), 0);
            if (buf == THNX)
                printf("THNX received \n");
            return (void *) 0;
        }
    }
}

double evaluate(my_task_t *task, int thr_num, int socket) {
    printf("In evaluate x1 = %.Lf, x2 = %Lf\n", task->x1, task->x2);

    pthread_t *pthreads_ids = malloc((thr_num + 1) * sizeof(pthread_t));
    my_task_t *thr_tasks = malloc(thr_num * sizeof(my_task_t));
    long double integral_sum = 0;
    long double length_per_thread = (task->x2 - task->x1) / thr_num;
    printf("length per thread: %Lf\n", length_per_thread);

    sock_and_sum_t sock_sum;
    sock_sum.socket = socket;
    sock_sum.sum = NULL;

    /* measure time calculating */

    struct timespec ts_start;
    struct timespec ts_end;
    double elapsed;
#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts_start.tv_sec = mts.tv_sec;
    ts_start.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, &ts_start);
#endif

    for (int i = 0; i < thr_num; i++) {
        thr_tasks[i].x1 = task->x1 + i * length_per_thread;
        thr_tasks[i].x2 = thr_tasks[i].x1 + length_per_thread;
        pthread_create(&pthreads_ids[i], NULL, thread_func, (void *) &thr_tasks[i]);
    }

    /* make thread for ping */

    pthread_create(&pthreads_ids[thr_num], NULL, ping_thread, (void *) &sock_sum);

    for (int i = 0; i < thr_num; i++) {
        pthread_join(pthreads_ids[i], NULL);
        integral_sum += thr_tasks[i].x1;
    }

#ifdef __MACH__

    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts_end.tv_sec = mts.tv_sec;
    ts_end.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, &ts_start);
#endif

    elapsed = (ts_end.tv_sec - ts_start.tv_sec);
    elapsed += (ts_end.tv_nsec - ts_start.tv_nsec)/1000000000.0;

    /* measure time complited */

    printf("CALCULATION COMPLITED! res = %Lf\n", integral_sum);
    sock_sum.sum = malloc(sizeof(long double));
    *sock_sum.sum = integral_sum;
    int ping_return = 0;

    pthread_join(pthreads_ids[thr_num], (void *) &ping_return);
    if (ping_return) {
        free(pthreads_ids);
        free(thr_tasks);
        free(sock_sum.sum);
        return 1;
    }

    free(sock_sum.sum);
    free(pthreads_ids);
    free(thr_tasks);
    printf("it took %lf seconds\n", elapsed);
    return 0;
}