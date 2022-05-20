//高、中、低优先极线程。临时线程,用于临时定时任务
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "pool.h"
#include "../queue/queue.h"
#include "base.h"
#include <stdbool.h>
// gcc pool.c ../queue/queue.c -lpthread ../include/base.c -I ../include/
// 最大线程数量
// 紧急任务 数量
// 普通任务数量
// 

typedef struct THREADPOOL_S {
    pthread_mutex_t mutex;

    pthread_cond_t normal_cond;

    int max_thread_num; // 最大线程数量

    int normal_thread_num; // 普通任务数量

    QUEUE_HANDLE normal_task_queue; // 普通任务队列

    QUEUE_HANDLE normal_thread_queue; // 普通线程队列

    bool fast_cond_flag;
    bool normal_cond_flag;

} THREADPOOL_S;

// 任务函数
typedef void (*run_func) (void *);

typedef struct task_node_s{
    run_func func;
    void *data; 
} TASK_NODE_S; 

//任务队列模式的线程池

// 普通线程函数
static void* __normal_thread_func(void *arg)
{
    THREADPOOL_S *p_pool = (THREADPOOL_S *)arg;

    while(1) {
        pthread_mutex_lock(&p_pool->mutex);

        printf("thread_pool normal_thread wait cond, tid:%lu\n", pthread_self());
        while (test_queue_depth_get(p_pool->normal_task_queue) == 0) { 
            pthread_cond_wait(&p_pool->normal_cond, &p_pool->mutex);
        }
        printf("thread_pool normal_thread get cond, tid:%lu\n", pthread_self());

        // 获取到条件变量
        TASK_NODE_S *node  = (TASK_NODE_S *)test_out_queue_malloc(p_pool->normal_task_queue);
        if  (node == NULL) {
            printf("node is NULL\n");
            goto ERR_EXIT;
        }

        pthread_mutex_unlock(&p_pool->mutex); // 解锁后再执行，防止死锁

        node->func(node->data);
    }

    return NULL;
ERR_EXIT:
    printf("__normal_thread_func err_exit");
    pthread_mutex_unlock(&p_pool->mutex); // 解锁后再执行，防止死锁
    return ((void *)-1);
}

static int __create_threads(THREADPOOL_S *p_pool)
{
    int ret = 0;

    pthread_attr_t attr = {0};
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //分离模式
    pthread_attr_setschedpolicy(&attr, SCHED_RR);// 实时时间片轮转调度

    int i = 0;

    pthread_t *normal_thread = NULL;
    for (i = 0; i < p_pool->normal_thread_num; i++) {
        normal_thread = (pthread_t *)malloc(sizeof(pthread_t));
        if (normal_thread == NULL) {
            printf("malloc failed\n");
            goto ERR_EXIT;
        }

        ret = pthread_create(normal_thread, &attr, __normal_thread_func, p_pool);
        if (ret != 0) {
            goto ERR_EXIT;
        }
        test_in_queue_malloc(p_pool->normal_thread_queue, NULL);
    }

    return 0;
ERR_EXIT:
    MY_PRINTF("thread_pool __create_threads err_exit\n");
    return -1;
}

// 线程池初始化
THREADPOOL_S* thread_pool_init()
{
    int ret = 0;

    THREADPOOL_S *p_pool = (THREADPOOL_S *)malloc(sizeof(THREADPOOL_S));
    if (p_pool == NULL) {
        return NULL;
    }
    memset(p_pool, 0, sizeof(THREADPOOL_S));

    // 初始化锁和信号量
    ret = pthread_mutex_init(&p_pool->mutex, NULL);
    if (ret != 0) {
        printf("pthread_mutex_init failed\n");
        return NULL;
    }
    ret = pthread_cond_init(&p_pool->normal_cond, NULL);
    if (ret != 0) {
        printf("pthread_cond_init failed\n");
        return NULL;
    }

    p_pool->max_thread_num = 5;
    p_pool->normal_thread_num = 5;

    p_pool->normal_task_queue = test_queue_init(sizeof(TASK_NODE_S), 20, NULL);
    p_pool->normal_thread_queue = test_queue_init(0, 20, NULL);

    printf("prepare to create threads\n");
    ret = __create_threads(p_pool);
    if (ret != 0) {
        printf("__create_threads failed\n");
        return NULL;
    }
    printf("create threads successfully\n");
    return p_pool;
}

// 添加任务
int thread_pool_add_task(THREADPOOL_S *p_pool, run_func func, void *arg)
{
    CHECK_NULL_RETURN_VAL(p_pool);

    pthread_mutex_lock(&p_pool->mutex);

    TASK_NODE_S node = {0};
    node.data = arg;
    node.func = func;

    test_in_queue_malloc(p_pool->normal_task_queue, (void *)&node);

    pthread_cond_signal(&p_pool->normal_cond);

    pthread_mutex_unlock(&p_pool->mutex);
    return 0;
}



//处理方法1
void test_func(void *data)
{
    int *i = (int *)data;

    printf("test_func fast-thread i is %d, tid is :%ld\n", *i, pthread_self());
    sleep(rand() % 3);// 为了看出线程在轮换
}


int main(int argc, char **argv)
{
    int arr[50] = {0};
    THREADPOOL_S *p_pool = thread_pool_init();
    sleep(2);
    for (int i = 0; i < 10; i++)
    {
        arr[i] = i;
        thread_pool_add_task(p_pool, test_func, (void *)&arr[i]); //TODO 1)add队列满处理 2)记录有没有空闲线程，防止发送空信号
        //sleep(1);
    }
    sleep(1000);
    
    return 0;
}
