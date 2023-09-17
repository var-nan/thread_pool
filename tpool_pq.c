#include "tpool_pq.h"

int init_tpool_pq() {
    tpool_pq_t tpool_queue;
}

// TODO: implement heap property.

// void swap(tpool_pq_t *tpool_queue, )

int insert_task(tpool_pq_t *tpool_queue, task_t task) {
    
    pthread_mutex_lock(&(tpool_queue->mutex));
    
    if (tpool_queue->n_tasks < MAX_QUEUE_SIZE) {
        // insert task into queue
        tpool_queue->task_array[tpool_queue->n_tasks++] = task;
        // TODO: restore heap property.
    }

    // release lock
    pthread_mutex_unlock(&(tpool_queue->mutex));
    return 1;
}

task_t get_task(tpool_pq_t *tpool_queue) {
    
    task_t task;
    pthread_mutex_lock(&(tpool_queue->mutex));
    if (tpool_queue->n_tasks < MAX_QUEUE_SIZE) {
        // insert task into queue
        task = tpool_queue->task_array[0];
        // decrease counter
        tpool_queue->n_tasks--;

        /*TODO: restore heap*/
    }
    
    pthread_mutex_unlock(&(tpool_queue->mutex));

    return task;
    
}

int get_num_tasks(tpool_pq_t *tpool_queue) {
    pthread_mutex_lock(&tpool_queue->mutex);
    int n = tpool_queue->n_tasks;
    pthread_mutex_unlock(&tpool_queue->mutex);
    return n;
}