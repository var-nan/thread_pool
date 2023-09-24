#include <pthread.h>

#ifndef TASK_QUEUE_TYPES

#define TASK_QUEUE_TYPES 1

typedef enum priority {LOW = 1, MEDIUM = 3, HIGH = 5} priority_t;

typedef struct task {
    void (*function)(int);
    void *arguments;
    priority_t priority;
} task_t;

typedef struct tpool_pq {
    task_t *task_array;
    int n_tasks;
    int queue_size;
} tpool_pq_t;

#endif

// functions for priority queue
int init_tpool_pq(tpool_pq_t **pq, int queue_size);

//int init_tpool_pq_list(tpool_pq_t *pq, int queue_size, tast_t *task_list);

int insert_task(tpool_pq_t **pq,task_t task);

task_t get_task(tpool_pq_t **tpool);

int get_num_tasks(const tpool_pq_t *tpool);

void print_tasks(tpool_pq_t *tpool_pq);
