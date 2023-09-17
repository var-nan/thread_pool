#include <pthread.h>

#define MAX_QUEUE_SIZE 64
#define CONSTANT 10

typedef enum priority {LOW, MEDIUM, HIGH} priority_t;

typedef struct task_t {
    void (*function)();
    void *arguments;
    //struct task_t *next;
    priority_t priority;
} task_t;

/* TODO: remove static declarations */
typedef struct tpool_pq {
    task_t task_array[MAX_QUEUE_SIZE];
    int n_tasks = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
} tpool_pq_t;


// functions for priority queue
int init_tpool_pq(tpool_pq_t *pq);

int insert_task(tpool_pq_t *pq,task_t task);

task_t *get_task(tpool_pq_t *tpool);

int get_num_tasks(const tpool_pq_t *tpool);
