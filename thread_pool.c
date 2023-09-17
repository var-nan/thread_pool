#include <pthread.h>
#include "thread_pool.h"
#include <stdio.h>


void thread_pool_start(thread_pool_t *tpool) {
    // TODO:  in a loop, continously check work queue and execute tasks.
}

int thread_pool_init(thread_pool_t *tpool, int num_threads, int max_queue_size) {

    /*allocate size for tpool and initialize*/
    //thread_pool_t pool;

    int status;

    if ((tpool = (thread_pool_t *) malloc(sizeof(struct thread_pool))) == NULL) {
        perror("Malloc");
        exit(-1);
    }

    // initialize field members and threads
    tpool->num_threads = num_threads;
    tpool->cur_queue_size = 0;
    tpool->max_queue_size = max_queue_size;
    tpool->queue_closed = 0;
    tpool->shutdown = 0;

    // define and initialize threads
    // TODO: setup performance test for optimal number of threads.

    if ((tpool->threads = (pthread_t *)malloc(sizeof(pthread_t)*num_threads)) == NULL) {
        perror("Malloc: Pthread initialization");
        exit(-1);
    }

    // create threads
    for (int i = 0; i < num_threads; i++) {
        if ((status = pthread_create(&tpool->threads[i], NULL, NULL, NULL)) != 0) 
            fprintf(stderr, "pthread_create %s",strerror(status)), exit(-1); 
            // TODO: add waiting function to thread.
    }

    //initialize conditional variables
    if ((status = pthread_mutex_init(&(tpool->mutex), NULL))!= 0)
        fprintf(stderr, "pthread_mutex_init %s", strerror(status)), exit(-1);
    if ((status = pthread_cond_init(&(tpool->queue_empty), NULL)) != 0)
        fprintf(stderr, "pthread_cond_init %s", strerror(status) ), exit(-1);
    
    if ((status = pthread_cond_init(&(tpool->queue_full), NULL)) != 0)
        fprintf(stderr, "pthread_cond_init &s", strerror(status)), exit(-1);

    if ((status = pthread_cond_init(&(tpool->queue_not_empty), NULL)) != 0)
        fprintf(stderr, "pthread_cond_init %s", strerror(status)), exit(-1);
    
    return 1; // TODO: modify return value.
}


int thread_pool_init(thread_pool_t *tpool, int num_thread) {
    return thread_pool_init(tpool, num_thread, 256);
}

int submit_task(thread_pool_t *tpool, task_t task) {
    
    //task_t task = {function, args, priority};

    // lock on task queue
    pthread_mutex_lock(&(tpool->mutex));

    // TODO: UNECESSARY LOCK
    pthread_mutex_lock(&(tpool->task_queue->mutex));

    
    /* Block if queue is full*/
    while ((tpool->cur_queue_size >= tpool->max_queue_size)) {
        pthread_cond_wait(&(tpool->queue_full), &tpool->mutex);
    }

    if (tpool->shutdown || tpool->queue_closed){
        pthread_mutex_unlock(&(tpool->mutex));
        return -1;
    }

    if (tpool->cur_queue_size < tpool->max_queue_size) {    
        insert_task(tpool->task_queue, task);
        tpool->cur_queue_size++;
        if (tpool->cur_queue_size == 1)
            pthread_cond_signal(&(tpool->queue_not_empty));
    }
    
    pthread_mutex_unlock(&(tpool->mutex));
    
    return 0;    
}

int submit_task_list(thread_pool_t *tpool, task_t *tasks, int n) {

    /*
    1. Get lock on task queue
    2. Submit task in a loop
    3. Wait if task queue is full
    4. Release lock on task queue. 
    */
    
    pthread_mutex_lock(&(tpool->mutex));

    for (int i = 0; i < n ; i++) {
        while (tpool->cur_queue_size >= tpool->max_queue_size) {
            pthread_cond_wait(&(tpool->queue_full), &(tpool->mutex));
        }

        if (tpool->shutdown || tpool->queue_closed){
            pthread_mutex_unlock(&(tpool->mutex));
            return -1;
        }

        if (tpool->cur_queue_size < tpool->max_queue_size) {    
            insert_task(tpool->task_queue, *(tasks+i));
            tpool->cur_queue_size++;

            if (tpool->cur_queue_size == 1)
                pthread_cond_signal(&(tpool->queue_not_empty));
        }  
    }

    pthread_mutex_unlock(&(tpool->mutex));
    return 0;
}

int shutdown_tpool(thread_pool_t *tpool, int finish) {
    return 0;
}