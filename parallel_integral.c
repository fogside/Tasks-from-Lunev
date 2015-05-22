//
//  parallel
//
//  Created by Евгения Ческидова on 19/04/15.
//  Copyright (c) 2015 ___FOGSIDE___. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#define D 30.0

//volatile int running_threads = 0;

struct start_end_point
{
    double start_point;
    double end_point;
};


void *thread_func(void *structure)
{
    // printf("here %d\n", (int)count);
    double start_point = (*(struct start_end_point *)structure).start_point;
    double end_point = (*(struct start_end_point *)structure).end_point;
    double counter = start_point;
    double dx = 0.00000001;
    double *sum = (double*)calloc(1,sizeof(double));

    int i = 0;
    while(counter<end_point)
    {
        *sum += counter*counter*dx;
        counter+=dx;
        i++;
    }
    //printf("sum: %lf, I:, Iterations: %d\n", *sum, i);
    pthread_exit((void *)sum);
}


int main(int argc, const char * argv[]) {

    int n;
    //printf("I'm here!!!\n");
    if(argc>0)
        n = (int) strtol(argv[1], NULL, 10);
    else
    {
        printf("Stupid yellow worm! \nPlease say the number of threads:\n");
        return 0;
    }


    pthread_t *pthreads_ids = malloc(n*sizeof(pthread_t));
    struct start_end_point *points = calloc(n, sizeof(struct start_end_point));
    double *value_ptr;
    double integral_sum = 0;


    for(int i = 0; i<n; i++)
    {
        points[i].start_point = (D/n)*i;
        points[i].end_point = points[i].start_point + (D/n);
        pthread_create(&pthreads_ids[i], NULL, thread_func,(void *)&(points[i]));
    }
    for(int i = 0; i<n; i++)
    {
        pthread_join(pthreads_ids[i], (void**)&value_ptr);
        integral_sum += *value_ptr;
        free(value_ptr);
    }

    printf("%lf\n",integral_sum);
    free(pthreads_ids);
    free(points);

    return 0;
}