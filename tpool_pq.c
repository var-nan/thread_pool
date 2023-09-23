#include "tpool_pq.h"
#include <stdlib.h>
#include <stdio.h>


/* TODO: add logging. */
/* TODO: add fn to print task queue.*/


void swap(task_t *tasks, int i, int j) {
    task_t temp = (*(tasks+i));
    (*(tasks+i)) = (*(tasks+j));
    (*(tasks+j)) = temp;
}

/* functions to maintain heap property*/
void heapify_down(task_t *tasks, int heap_size, int i ) {
    int left = 2* i;
    int right = left + 1;
    int largest = i;
    
    /* compare priorities of childs and swap*/
    if (left <= heap_size && (((tasks+left))->priority > ((tasks+i))->priority))
        largest = left;

    if (right <= heap_size && (((tasks+right))->priority > ((tasks+largest))->priority)) 
        largest = right;
    
    if (largest != i && largest < heap_size){
        swap(tasks, i, largest);
        heapify_down(tasks, heap_size, largest);
    }
}

void heapify_up(task_t *tasks, int heap_size, int i) {
    int parent = (int) i/2;
    int largest = parent;

    if (parent <= heap_size && i <= heap_size  
        && (((tasks+i))->priority > ((tasks+parent))->priority)) 
        largest = i;
    
    if (largest != parent) {
        swap(tasks, parent, largest);
        /* heapify up if largest is not root*/
        if (largest > 0)
            heapify_up(tasks, heap_size, largest);
    }
}

void build_heap(task_t *tasks, int n_tasks){
    int start = 1 + (int) (n_tasks)/2;

    for (int i = start; i >= 0; i--) {
        heapify_down(tasks, n_tasks, i);
    }
}

/*shadow function for inti_tpool_pq */
int shadow_init_tpool_pq(tpool_pq_t **tpool_pqs, int queue_size) {

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



int init_tpool_pq(tpool_pq_t **tpool_pqs, int queue_size) {

    tpool_pq_t *tpool_pq;
    if ((tpool_pq = (tpool_pq_t *) malloc(sizeof(struct tpool_pq))) == NULL)
        printf("malloc returned null\n");
    
    tpool_pq->n_tasks = 0;
    tpool_pq->queue_size = queue_size;
    tpool_pq->task_array = (task_t *)malloc(sizeof(task_t)*queue_size);

    /* call build heap only if some tasks available */
    if (tpool_pq->n_tasks > 0)
        build_heap(tpool_pq->task_array, tpool_pq->n_tasks);
        
    *tpool_pqs = tpool_pq;
}



int insert_task(tpool_pq_t **tpool_queue, task_t task) {
    
    if ((*tpool_queue)->n_tasks < (*tpool_queue)->queue_size) {
        // insert task into queue
        (*tpool_queue)->task_array[(*tpool_queue)->n_tasks++ ] = task;
        // TODO: restore heap property.
        int last_inserted = (*tpool_queue)->n_tasks - 1;
        heapify_up((*tpool_queue)->task_array, (*tpool_queue)->queue_size, last_inserted);
        return 1;
    }
    /* insert not successful. */
    return -1;
}

task_t get_task(tpool_pq_t **tpool_queue) {
    
    task_t task;

    int root = 0;
    int last_inserted_index = (*tpool_queue)->n_tasks - 1;

    if ((*tpool_queue)->n_tasks < MAX_QUEUE_SIZE) {
        // insert task into queue
        task = (*tpool_queue)->task_array[0];

        // swap root task with last task.
        swap((*tpool_queue)->task_array, root, last_inserted_index);
        // decrease counter
        (*tpool_queue)->n_tasks--;

        /*TODO: restore heap property.*/
        heapify_down((*tpool_queue)->task_array, (*tpool_queue)->n_tasks, root);
    }
    
    return task;    
}

int get_num_tasks(const tpool_pq_t *tpool_queue) {
    
    int n = tpool_queue->n_tasks;
    return n;
}

void print_tasks(tpool_pq_t *tpool_pq) {
    int n = tpool_pq->n_tasks;

    for (int i = 0; i < n; i++) {
        task_t t = tpool_pq->task_array[i];
        //(*t.function)(t.arguments);
        //printf("Task, priority: %d\n", t.priority);
        
        //printf("Task: %d, priority: %d\n", i, tpool_pq->task_array[i].priority);
    }
}