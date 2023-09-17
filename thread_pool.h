#include <stdlib.h>
#include <pthread.h>
#include "tpool_pq.h"

#define MAX_QUEUE_SIZE 64
#define MAX_NUMBER_OF_THREADS 8

// define work_queue
typedef struct thread_pool {
    // thread_pool class
    int num_threads;
    int max_queue_size;
    int cur_queue_size;

    pthread_t *threads;
    tpool_pq_t *task_queue;
    pthread_mutex_t *mutex;
    pthread_cond_t queue_full;
    pthread_cond_t queue_empty;
    pthread_cond_t queue_not_empty;
    int queue_closed;
    int shutdown;
} thread_pool_t;

/*declare thread_pool interface*/
int thread_pool_init(thread_pool_t *tpool, int num_thread);
int thread_pool_init(thread_pool_t *tpool, int num_threads, int max_queue_size);

int submit_task(thread_pool_t *tpool, void *function, void *args);
int sumbit_task_list(thread_pool_t *tpool, task_t *task_list);

int shutdown_tpool(thread_pool_t *tpool, int finish);

