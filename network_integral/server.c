//
//  main.c
//  server
//
//  Created by Евгения Ческидова on 03/07/15.
//  Copyright (c) 2015 ___FOGSIDE___. All rights reserved.
//

#include "server.h"
#include "arpa/inet.h"

void send_broadcast();

int tcp_connection_accept(int socket_listen, worker_t *workers);

int tcp_get_thread_info(worker_t *workers, int N);

void distribution_evaluation(long double a, long double b, worker_t *workers, int N);

void send_tasks(worker_t *workers, int N);

int wait_for_tasks(worker_t *workers, int N, long double *result_sum);

void doomsday(worker_t *workers, int N, int failed_num);

int get_listening_socket();

int get_broadcast_socket();


int main(int argc, const char *argv[])
{
//    int N = 7;
//    worker_t workers[10] = {0};
//
//    for(int i = 0; i < N; i++)
//        workers[i].thr_num = 4;
//    workers[0].thr_num = 2;
//    double a = 0.0;
//    double b = 1000.0;
//    distribution_evaluation(a, b, workers, N);

    if (argc < 3)
    {
        printf("Введите промежуток интегрирования в аргументы командной строки: x1 и x2 \n");
        return 1;
    }

    double start_point;
    double end_point;
    start_point = strtod(argv[1], NULL);
    if(errno!= 0)
    { 
        printf("Incorrect value. Please, write correct value to x1\n");
        exit(1);
    }
    end_point = strtod(argv[2], NULL);
    if(errno!= 0)
    {
        printf("Incorrect value. Please, write correct value to x1\n");
        exit(1);
    }
    int N;  /* how many workers do we have */
    int numb_worked;  /* how many available workers do we have now */
    worker_t workers[PENDING_QUEUE_SIZE] = {0};
    long double sum_result = 0;

    int socket_listen = get_listening_socket();
    printf("turn on listen socket...\n");
    ASSERT(listen(socket_listen, PENDING_QUEUE_SIZE) != -1);
    send_broadcast();
    printf("waiting for response...\n");
    //sleep(CLIENT_LISTEN_TIMEOUT);   /* wait for response */
    fflush(NULL);

    N = tcp_connection_accept(socket_listen, workers);
    numb_worked = tcp_get_thread_info(workers, N);
    if (numb_worked < 1)
    {
        printf("Sorry guys, we don't have any workers!\n");
        return 0;
    }

    distribution_evaluation(start_point, end_point, workers, N);
    send_tasks(workers, N);
    int res = wait_for_tasks(workers, N, &sum_result);
    /* if smth failed we should die */
    if (res)
        return 1;
    else
        printf("result: %Lf\n", sum_result);
    return 0;
}

void send_broadcast()
{
    int socket_bound = get_broadcast_socket();

    //   struct in_addr broadcast_addr;
    // inet_aton("192.168.1.255", &broadcast_addr);

    struct sockaddr_in addr_broad = {
            .sin_family = AF_INET,
            .sin_port = htons(HANDSHAKE_PORT),
            .sin_addr.s_addr = INADDR_BROADCAST
    };
    ASSERT(sendto(socket_bound, &BROADCAST, sizeof(BROADCAST), 0, (struct sockaddr *) &addr_broad,
            sizeof(addr_broad)) == sizeof(BROADCAST));
    printf("I sent broadcast:\"Is there anybody in my little local network?\"\n");
    ASSERT(!close(socket_bound));
}

int tcp_connection_accept(int socket_listen, worker_t *workers)
{
    int curr_w_number = 0;

    for (int i = 0; i < PENDING_QUEUE_SIZE; i++)
        workers[i].socket = -1;
    printf("starting to accept tcp connection...\n");

//    while ((curr_w_number < PENDING_QUEUE_SIZE) && ( (tmp_socket = accept(socket_listen, NULL, 0)) != -1))

    fd_set active_fd_set, read_fd_set;
    FD_ZERO (&active_fd_set);
    FD_SET (socket_listen, &active_fd_set);

    struct timeval timeout = {
            .tv_sec = 10
    };
    while (curr_w_number < PENDING_QUEUE_SIZE)
    {
        /* Block until input arrives on one or more active sockets. */
        read_fd_set = active_fd_set;
        int ret = select(FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout);
        if (ret < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            printf("Timeout selecting\n");
            break;
        }

        if (FD_ISSET(socket_listen, &read_fd_set))
        {
            /* Connection request on original socket. */
            int new = accept(socket_listen, NULL, NULL);
            if (new < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            workers[curr_w_number].socket = new;
            curr_w_number++;
        }
        else
        {
            printf("wtf o_O");
        }
    }

    printf("N = %d connections accepted\n", curr_w_number);

    ASSERT(!close(socket_listen));  /* to finish listen */
    return curr_w_number;
}

int tcp_get_thread_info(worker_t *workers, int N)
{
    ssize_t length;
    int work_numb = 0;
    int buf = 0;

    printf("starting to getting thread info...\n");
    for (int i = 0; i < N; i++)
    {
        length = recv(workers[i].socket, (void *) &buf, sizeof(int), 0);
        if ((length == -1) || (length == 0))
        {
            printf("Worker number %d doesn't want to work!", i);
            workers[i].thr_num = -1;
            // TODO: Maybe a better way?
        }
        else
        {
            workers[i].thr_num = buf;
            work_numb++;
        }
    }
    printf("got thread info.\n work_numb = %d\n", work_numb);
    return work_numb;
}

void distribution_evaluation(long double a, long double b, worker_t *workers, int N)
{
    /* Here there is a distribution of evaluation of integral
    from x1 = a to x2 = b */

    printf("distribution evaluation...\n");
    int curr_thr_num = 0; /* number of current threads */
    double total_threads = 0;
    for (int i = 0; i < N; i++)
    {
        if (workers[i].thr_num != -1)
            total_threads += workers[i].thr_num;
    }

    long double length_per_thr = (b - a) / total_threads;

    for (int i = 0; i < N; i++)
    {
        if (workers[i].thr_num != -1)
        {
            workers[i].task.x1 = a + (curr_thr_num * length_per_thr);
            workers[i].task.x2 = workers[i].task.x1 + workers[i].thr_num * length_per_thr;
            curr_thr_num += workers[i].thr_num;

            printf("workers[%d] received x1 = %Lf, x2 = %Lf\n", i, workers[i].task.x1, workers[i].task.x2);
        }
    }
}

void send_tasks(worker_t *workers, int N)
{
    printf("sending tasks...\n");
    for (int i = 0; i < N; ++i)
    {
        worker_t worker = workers[i];
        if (worker.thr_num == -1)
            continue;
        printf("task for %d: x1= %Lf, x2 =%Lf\n", i, worker.task.x1, worker.task.x2);
        ssize_t bytes_sent = send(worker.socket, &worker.task, sizeof(my_task_t), 0);
        if (bytes_sent != sizeof(my_task_t))
        {
            printf("in send_tasks send task to %d worker failed\n", i);
            doomsday(workers, N, 0);
        }
    }
    printf("tasks were sent\n");
}


int wait_for_tasks(worker_t *workers, int N, long double *result_sum)
{

    printf("starting waiting for solved tasks...\n");
    fflush(NULL);
    int count = 0;
    while (count < N)
    {
        count = 0;
        for (int i = 0; i < N; i++)
        {
            worker_t *worker = workers + i;
            if ((worker->thr_num == -1) || worker->finished)
            {
                count++;
                continue;
            }

            ssize_t sent_bytes = send(worker->socket, &PING, sizeof(PING), 0);
            if (sent_bytes != sizeof(PING))
            {
                printf("in wait_for_task send ping failed\n");
                doomsday(workers, N, i);
                return 1;
            }
            int incoming = 0;
            ssize_t received_bytes = 0;
            do
            {
                received_bytes = recv(worker->socket, &incoming, sizeof(incoming), 0);
            } while (received_bytes <= 0 && errno == ETIMEDOUT);

            if (received_bytes != sizeof(incoming))
            {
                printf("in wait_for_task receive incoming failed\n");
                doomsday(workers, N, i);
                return 1;
            }

            printf("received incoming = %d\n", incoming);
            fflush(NULL);
            if (incoming == 1) /* still working */
            {
                continue;
            }
            else if (incoming == 2) /* done */
            {

                ssize_t result_bytes = recv(worker->socket, &(worker->result), sizeof(worker->result), 0);
                if (result_bytes != sizeof(worker->result))
                {
                    printf("in wait_for_task receiving result from %d failed\n", i);
                    doomsday(workers, N, i);
                    return 1;
                }
                printf("worker number %d finished\n", i);
                printf("his result is %Lf\n", worker ->result);
                printf("sending THNX...\n");
                send(worker->socket, &THNX, sizeof(THNX), 0);

                fflush(NULL);
                *result_sum += worker->result;
                worker->finished = 1;
                count++;
            }
            else
            {
                printf("in wait_for_task bad value was received\n");
                doomsday(workers, N, i);
                return 1;
            }
        }
        sleep(1);
    }
    printf("results were received (!) \n");
    fflush(NULL);
    return 0;
}

void doomsday(worker_t *workers, int N, int failed_num)
{
    printf("The doomsday is coming...(!)\n");
    for (int i = 0; i < N; ++i)
    {
        if (workers[i].finished || i == failed_num)
            continue;
        send(workers[i].socket, &KILL, sizeof(KILL), 0);
    }
}

int get_listening_socket()
{
    printf("getting listening socket...\n");
    int sock_lstn = socket(AF_INET, SOCK_STREAM, 0);    /*the last argument is protocol, which is default here*/
    ASSERT(sock_lstn != -1);
    struct sockaddr_in listening_addr;
    listening_addr.sin_family = AF_INET;
    listening_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listening_addr.sin_port = htons(LSTN_PORT);
    ASSERT(!bind(sock_lstn, (struct sockaddr *) &listening_addr, sizeof(listening_addr)));
    return sock_lstn;
}

int get_broadcast_socket()
{
    printf("getting broadcast socket...\n");
    int sock_broadcast = socket(AF_INET, SOCK_DGRAM, 0);    /*the last argument is protocol, which is default here*/
    ASSERT(sock_broadcast != -1);
    int broadcastEnable = 1;
    int ret = setsockopt(sock_broadcast, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    ASSERT(!ret);
    return sock_broadcast;
}

