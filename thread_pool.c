#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "thread_pool.h"
#define _GNU_SOURCE
#include <unistd.h>


/*TODO: add logging. */

void thread_pool_start(thread_pool_t *tpool) {
    
    task_t task;

    /* continously poll the task_queue to get task. */
    for (;;) {
        /* acquire lock*/
        pthread_mutex_lock((tpool->mutex));

        /* wait on empty task queue. */
        while ((tpool->cur_queue_size == 0) && !(tpool->shutdown)) {
            pthread_cond_wait(&(tpool->queue_not_empty), (tpool->mutex));
        }

        if (tpool->shutdown) {
            pthread_mutex_unlock((tpool->mutex));
            pthread_exit(NULL); /*TODO: add return value*/
        }
        
        task = get_task(&(tpool->task_queue));

        /* decrease current queue size. Assuming one thread gets only one task. */
        tpool->cur_queue_size--;

        
        /* Signal if task queue is emtpy. Used during shutdown. */
        if (tpool->cur_queue_size == 0)
            pthread_cond_signal(&(tpool->queue_empty));        

        /* Signal if writing thread is waiting to add task.*/
        if (tpool->cur_queue_size == tpool->max_queue_size -1)
            pthread_cond_signal(&(tpool->queue_not_full));
        
        pthread_mutex_unlock((tpool->mutex));

        printf("Executing task priority: %d by thread:%d\n", task.priority, gettid());
        /*execute task*/
        //(task.function)(task.arguments); /* TODO: modify this. */
    }
}

int thread_pool_init_qs(thread_pool_t **thr_pool, int num_threads, int max_queue_size) {

    /*allocate size for tpool and initialize */

    int status;
    thread_pool_t *tpool;

    /* allocate memory for thread_pool. */
    if ((tpool = (thread_pool_t *) malloc(sizeof(struct thread_pool))) == NULL) {
        perror("Malloc");
        exit(-1);
    }

    /*allocate memory for mutex. */
    (tpool->mutex) = malloc(sizeof(pthread_mutex_t));

    /* Declare memory for threads */
    if ((tpool->threads = (pthread_t *)malloc(sizeof(pthread_t)*num_threads)) == NULL) {
        perror("Malloc: Pthread initialization");
        exit(-1);
    }

    /* initialize tpool_pq_t, also allocates memory*/
    init_tpool_pq(&tpool->task_queue, max_queue_size);

    // initialize field members and threads
    tpool->num_threads = num_threads;
    tpool->cur_queue_size = 0;
    tpool->max_queue_size = max_queue_size;
    tpool->queue_closed = 0;
    tpool->shutdown = 0;

    // TODO: setup performance test for optimal number of threads.


    
    /* initialize conditional variables */
    if ((status = pthread_mutex_init((tpool->mutex), NULL))!= 0)
        fprintf(stderr, "pthread_mutex_init %s", strerror(status)), exit(-1);
    if ((status = pthread_cond_init(&(tpool->queue_empty), NULL)) != 0)
        fprintf(stderr, "pthread_cond_init %s", strerror(status) ), exit(-1);
    
    if ((status = pthread_cond_init(&(tpool->queue_not_full), NULL)) != 0)
        fprintf(stderr, "pthread_cond_init %s", strerror(status)), exit(-1);

    if ((status = pthread_cond_init(&(tpool->queue_not_empty), NULL)) != 0)
        fprintf(stderr, "pthread_cond_init %s", strerror(status)), exit(-1);

    /* Waiting function for threads. */
    for (int i = 0; i < num_threads; i++) {
        /* */
        if ((status = pthread_create(&tpool->threads[i], NULL, (void *)thread_pool_start, tpool)) != 0) 
            fprintf(stderr, "pthread_create %s",strerror(status)), exit(-1); 
    }

    *thr_pool = tpool;
    
    return 1;
}



int thread_pool_submit_task(thread_pool_t **tpool, task_t task) {
    
    //task_t task = {function, args, priority};

    // lock on task queue
    pthread_mutex_lock((*tpool)->mutex);
    
    /* Block if queue is full*/
    while (((*tpool)->cur_queue_size >= (*tpool)->max_queue_size)) {
        pthread_cond_wait(&((*tpool)->queue_not_full), (*tpool)->mutex);
    }

    if ((*tpool)->shutdown || (*tpool)->queue_closed){
        pthread_mutex_unlock((*tpool)->mutex);
        return -1;
    }

    if ((*tpool)->cur_queue_size < (*tpool)->max_queue_size) {    
        if (insert_task(&(*tpool)->task_queue, task)) { // TODO: check return value of fn.
            (*tpool)->cur_queue_size++;
            /* Signal if other threads waiting on empty queue.*/
            if ((*tpool)->cur_queue_size == 1)
                pthread_cond_signal(&((*tpool)->queue_not_empty));
        } else {
            printf("Task is not inserted.\n");
        }
    }
    
    pthread_mutex_unlock((*tpool)->mutex);
    
    return 1;    
}

int thread_pool_submit_task_list(thread_pool_t **tpool, task_t *tasks, int n) {

    /*
    1. Get lock on task queue
    2. Submit task in a loop
    3. Wait if task queue is full
    4. Release lock on task queue. 
    */
    
    pthread_mutex_lock( (*tpool)->mutex );

    for (int i = 0; i < n ; i++) {
        while ((*tpool)->cur_queue_size >= (*tpool)->max_queue_size) {
            pthread_cond_wait(&((*tpool)->queue_not_full), (*tpool)->mutex);
        }

        if ( (*tpool)->shutdown || (*tpool)->queue_closed){
            pthread_mutex_unlock((*tpool)->mutex);
            return -1;
        }

        if ((*tpool)->cur_queue_size < (*tpool)->max_queue_size) {    
            if (insert_task(&(*tpool)->task_queue, *(tasks+i))) { // TODO: check return value of fn.
                (*tpool)->cur_queue_size++;

                /* Signal if other threads waiting on empty queue.*/
                if ((*tpool)->cur_queue_size == 1)
                    pthread_cond_signal(&((*tpool)->queue_not_empty));
            } else {
                printf("Task is not inserted.\n");
            }
        }  
    }

    pthread_mutex_unlock((*tpool)->mutex);
    return 0;
}

int thread_pool_shutdown(thread_pool_t **tpool, int finish) {
    
    int status;
    /* should return 0*/
    if((status = pthread_mutex_lock((*tpool)->mutex)) != 0 ) {
        fprintf(stderr, "pthread_mutex_lock %d", status );
        exit(-1);
    }

    printf("Shutdown initiated..\n");

    if ((*tpool)->shutdown || (*tpool)->queue_closed) {
        pthread_mutex_unlock((*tpool)->mutex);
        return 2;
    }

    /* set closed flag to 1. */
    (*tpool)->queue_closed = 1;
    printf("Task Queue closed..\n");

    /* wait for task queue to empty. */
    while( (*tpool)->cur_queue_size != 0) {
        pthread_cond_wait(&((*tpool)->queue_empty), (*tpool)->mutex);
    }

    (*tpool)->shutdown = 1;

    /*TODO: add error handling code. */
    if ((*tpool)->cur_queue_size == 0) {
        pthread_mutex_unlock((*tpool)->mutex);
    }

    /* add cleanup code*/

    /* wake up any threads to recheck shutdown flag */
    pthread_cond_broadcast(&((*tpool)->queue_not_empty));

    pthread_cond_broadcast(&((*tpool)->queue_not_full));


    /* wait for workers to finish. */
    for ( int i = 0; i < (*tpool)->num_threads; i++) {
        pthread_join((*tpool)->threads[i], NULL);
    }

    printf("Nubmer of tasks remaining: %d\n", (*tpool)->cur_queue_size);

    /* clean up resources. */
    free ((*tpool)->threads);
    free (*tpool);
    //free(tpool);
    
    return 1;
}