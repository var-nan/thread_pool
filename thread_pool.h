#include <stdlib.h>
#include <pthread.h>
#include "tpool_pq.h"

#define MAX_QUEUE_SIZE 64
#define MAX_NUMBER_OF_THREADS 8

// define work_queue
typedef struct thread_pool {
    /* Thread pool struct*/
    int num_threads;
    int max_queue_size;
    int cur_queue_size;

    pthread_t *threads;
    tpool_pq_t *task_queue;
    pthread_mutex_t *mutex;
    pthread_cond_t queue_not_full; /* Flag: threads wait if queue is full, threads signal if space is available.*/
    pthread_cond_t queue_empty; /* Flag: thread waits if queue is empty, thread signals if task is added. */
    pthread_cond_t queue_not_empty; /* Flag: threads signal if task is added.  */
    int queue_closed;
    int shutdown;
} thread_pool_t;

/*declare thread_pool interface*/
int thread_pool_init(thread_pool_t *tpool, int num_thread);
int thread_pool_init_qs(thread_pool_t *tpool, int num_threads, int max_queue_size);

int thread_pool_submit_task(thread_pool_t *tpool, task_t task);
int thread_pool_submit_task_list(thread_pool_t *tpool, task_t *task_list, int n);

int thread_pool_shutdown(thread_pool_t *tpool, int finish);

