#include <pthread.h>

#define MAX_QUEUE_SIZE 64
#define CONSTANT 10

typedef enum priority_t {LOW = 1, MEDIUM = 3, HIGH = 5} priority_t;

typedef struct task_t {
    void (*function)();
    void *arguments;
    /* struct task_t *next; */
    priority_t priority;
} task_t;

/* TODO: remove static declarations */
typedef struct tpool_pq {
    task_t *task_array; /* TODO: change to pointer. */
    int n_tasks;
    int queue_size;
    /* pthread_mutex_t mutex; */
} tpool_pq_t;


// functions for priority queue
int init_tpool_pq(tpool_pq_t **pq, int queue_size);

//int init_tpool_pq_list(tpool_pq_t *pq, int queue_size, tast_t *task_list);

int insert_task(tpool_pq_t **pq,task_t task);

task_t get_task(tpool_pq_t **tpool);

int get_num_tasks(const tpool_pq_t *tpool);

void print_tasks(tpool_pq_t *tpool_pq);
