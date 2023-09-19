#include "tpool_pq.h"
#include <stdlib.h>
#include <stdio.h>


/* TODO: add logging. */
/* TODO: add fn to print task queue.*/


void swap(task_t *tasks, int i, int j) {
    task_t temp = *(tasks+i);
    *(tasks+i) = *(tasks+j);
    *(tasks+j) = temp;
}

/* functions to maintain heap property*/
void heapify_down(task_t *tasks, int heap_size, int i ) {
    int left = 2* i;
    int right = left + 1;
    int largest = i;
    //int left_priority = (tasks+left)->priority;

    /* compare priorities of childs and swap*/
    if (left <= heap_size && ((tasks+left)->priority > (tasks+i)->priority))
        largest = left;

    if (right <= heap_size && ((tasks+right)->priority > (tasks+largest)->priority)) 
        largest = right;
    
    if (largest != i){
        swap(tasks, i, largest);
        heapify_down(tasks, heap_size, largest);
    }
}

void heapify_up(task_t *tasks, int heap_size, int i) {
    int parent = (int) i/2;
    int largest = parent;

    if (parent <= heap_size && (tasks+i)->priority > (tasks+parent)->priority) 
        largest = i;
    
    if (largest != parent) {
        swap(tasks, i, largest);
        /* heapify up if largest is not root*/
        if (largest > 0)
            heapify_up(tasks, heap_size, largest);
    }
}

void build_heap(task_t *tasks, int n_tasks){
    int start = (int) (n_tasks)/2;

    for (int i = start; i >= 0; i--) {
        heapify_down(tasks, n_tasks, i);
    }
}

int init_tpool_pq(tpool_pq_t **tpool_pqs, int queue_size) {

    //tpool_pq_t tpool_queue;
    tpool_pq_t *tpool_pq;
    if ((tpool_pq = (tpool_pq_t *) malloc(sizeof(struct tpool_pq))) == NULL)
        printf("malloc returned null\n");
    
    // TODO: change static initialization to dynamic initialization;
    tpool_pq->n_tasks = 0;
    tpool_pq->queue_size = queue_size;
    tpool_pq->task_array = (task_t *)malloc(sizeof(task_t)*queue_size);

    /* call build heap only if some tasks available. TODO:  */
    if (tpool_pq->n_tasks > 0)
        build_heap(tpool_pq->task_array, tpool_pq->n_tasks);
        
    *tpool_pqs = tpool_pq;
}



int insert_task(tpool_pq_t **tpool_queue, task_t task) {
    
    // pthread_mutex_lock(&(tpool_queue->mutex));
    // printf("Pool_queue: tasks: %d, queue_size = %d", tpool_queue->n_tasks, tpool_queue->queue_size);
    
    if ((*tpool_queue)->n_tasks < (*tpool_queue)->queue_size) {
        // insert task into queue
        int index = (*tpool_queue)->n_tasks;
        (*tpool_queue)->task_array[(*tpool_queue)->n_tasks++] = task;
        // TODO: restore heap property.
        int last_inserted = (*tpool_queue)->n_tasks;
        heapify_up((*tpool_queue)->task_array, 0, last_inserted);
    }

    // release lock
    // pthread_mutex_unlock(&(tpool_queue->mutex));
    return 1;
}

task_t get_task(tpool_pq_t **tpool_queue) {
    
    task_t task;
    // pthread_mutex_lock(&(tpool_queue->mutex));

    if ((*tpool_queue)->n_tasks < MAX_QUEUE_SIZE) {
        // insert task into queue
        task = (*tpool_queue)->task_array[0];

        // swap root task with last task.
        swap((*tpool_queue)->task_array, 0, ((*tpool_queue)->n_tasks)-1);
        // decrease counter
        (*tpool_queue)->n_tasks--;

        /*TODO: restore heap property.*/
        heapify_down((*tpool_queue)->task_array, (*tpool_queue)->n_tasks, 0);
    }
    
    // pthread_mutex_unlock(&(tpool_queue->mutex));
    return task;    
}

int get_num_tasks(const tpool_pq_t *tpool_queue) {
    //pthread_mutex_lock(&tpool_queue->mutex);
    int n = tpool_queue->n_tasks;
    //pthread_mutex_unlock(&tpool_queue->mutex);
    return n;
}

void print_tasks(tpool_pq_t *tpool_pq) {
    int n = tpool_pq->n_tasks;

    for (int i = 0; i < n; i++) {
        printf("Task: %d, priority: %d\n", i, tpool_pq->task_array[i].priority);
    }
}