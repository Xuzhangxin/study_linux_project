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
#include "queue.h"
#include "base.h"
#include <stdbool.h>

// 最大线程数量
// 紧急任务 数量
// 普通任务数量
// 

typedef struct THREADPOOL_S {
    pthread_mutex_t mutex;

    pthread_cond_t fast_cond;
    pthread_cond_t normal_cond;

    int max_thread_num; // 最大线程数量

    int fast_task_num; // 紧急任务数量
    int normal_task_num; // 普通任务数量

    QUEUE_S *fast_task_queue; // 紧急任务队列
    QUEUE_S *normal_task_queue; // 普通任务队列

    QUEUE_S *fast_thread_queue; // 紧急线程队列
    QUEUE_S *normal_thread_queue; // 普通线程队列

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

// 紧急线程函数
static void* __fast_thread_func(void *arg)
{
    THREADPOOL_S *p_pool = (THREADPOOL_S *)arg;

    while(1) {
        pthread_mutex_lock(&p_pool->mutex);

        // 接收到signal到wait函数返回有时间，可能fast_task_queue已被取走，所以要再次判断，
        while (p_pool->fast_task_queue->cnt == 0) {
            pthread_cond_wait(&p_pool->fast_cond, &p_pool->mutex);
        }

        // if (p_pool->fast_cond_flag == false) {
        //     continue;
        // }
        // 获取到条件变量
        TASK_NODE_S *node  = (TASK_NODE_S *)test_out_queue_malloc(p_pool->fast_task_queue); //没解锁
        //CHECK_NULL_RETURN_NULL(node);
        if  (node == NULL) {
            printf("node is NULL\n");
            goto ERR_EXIT;
        }

        node->func(node->data);
        p_pool->fast_cond_flag = false;
        pthread_mutex_unlock(&p_pool->mutex); // 解锁后再执行，防止死锁

    }

    return NULL;
ERR_EXIT:
    pthread_mutex_unlock(&p_pool->mutex); // 解锁后再执行，防止死锁
    return ((void *)-1);
}

// 普通线程函数
static void* __normal_thread_func(void *arg)
{
    THREADPOOL_S *p_pool = (THREADPOOL_S *)arg;

    while(1) {
        pthread_mutex_lock(&p_pool->mutex);

        while (p_pool->normal_task_queue->cnt == 0) { 
            pthread_cond_wait(&p_pool->normal_cond, &p_pool->mutex);
        }

        // if (p_pool->normal_cond_flag == false) {
        //     continue;
        // }
            // 获取到条件变量
            TASK_NODE_S *node  = (TASK_NODE_S *)test_out_queue_malloc(p_pool->normal_task_queue);
            if  (node == NULL) {
                printf("node is NULL\n");
                goto ERR_EXIT;
            }

            node->func(node->data);

        p_pool->normal_cond_flag = false;
        pthread_mutex_unlock(&p_pool->mutex); // 解锁后再执行，防止死锁
    }

    return NULL;
ERR_EXIT:
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
    struct sched_param param = {0};

    pthread_t *fast_thread = NULL;
    int i = 0;
    for (i = 0; i < p_pool->fast_task_num; i++) {
        fast_thread = (pthread_t *)malloc(sizeof(pthread_t));
        if (fast_thread == NULL) {
            printf("malloc failed\n");
            goto ERR_EXIT;
        }

        param.sched_priority = 60;
        pthread_attr_setschedparam(&attr, &param);

        ret = pthread_create(fast_thread, &attr, __fast_thread_func, p_pool);
        if (ret != 0) {
            goto ERR_EXIT;
        }
        test_in_queue_malloc(p_pool->fast_thread_queue, NULL);
    }

    pthread_t *normal_thread = NULL;
    for (i = 0; i < p_pool->normal_task_num; i++) {
        normal_thread = (pthread_t *)malloc(sizeof(pthread_t));
        if (normal_thread == NULL) {
            printf("malloc failed\n");
            goto ERR_EXIT;
        }

        param.sched_priority = 30;
        pthread_attr_setschedparam(&attr, &param);

        ret = pthread_create(normal_thread, &attr, __normal_thread_func, p_pool);
        if (ret != 0) {
            goto ERR_EXIT;
        }
        test_in_queue_malloc(p_pool->normal_thread_queue, NULL);
    }

    return 0;
ERR_EXIT:
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
    ret = pthread_cond_init(&p_pool->fast_cond, NULL);
    if (ret != 0) {
        printf("pthread_cond_init failed\n");
        return NULL;
    }
    ret = pthread_cond_init(&p_pool->normal_cond, NULL);
    if (ret != 0) {
        printf("pthread_cond_init failed\n");
        return NULL;
    }

    p_pool->max_thread_num = 3;
    p_pool->fast_task_num = 1;
    p_pool->normal_task_num = 2;

    p_pool->fast_task_queue = test_queue_init(sizeof(TASK_NODE_S));
    p_pool->normal_task_queue = test_queue_init(sizeof(TASK_NODE_S));

    p_pool->fast_thread_queue = test_queue_init(0);
    p_pool->normal_thread_queue = test_queue_init(0);

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
int thread_pool_add_task(THREADPOOL_S *p_pool, run_func func, void *arg, int if_fast)
{
    CHECK_NULL_RETURN_VAL(p_pool);

    pthread_mutex_lock(&p_pool->mutex);

    TASK_NODE_S node = {0};
    node.data = arg;
    node.func = func;

    if (if_fast) {
        test_in_queue_malloc(p_pool->fast_task_queue, (void *)&node);
    } else {
        test_in_queue_malloc(p_pool->normal_task_queue, (void *)&node);
    }

    if (if_fast) {
        while(p_pool->fast_cond_flag == true);
        p_pool->fast_cond_flag = true;
        pthread_cond_signal(&p_pool->fast_cond);

    } else {
        while(p_pool->normal_cond_flag == true);
        p_pool->normal_cond_flag = true;
        pthread_cond_signal(&p_pool->normal_cond);
    }

    pthread_mutex_unlock(&p_pool->mutex);
    return 0;
}



//处理方法1
void test_func(void *data)
{
    int *i = (int *)data;

    printf("test_func fast-thread i is %d, tid is :%ld\n", *i, pthread_self());
    // sleep(1);
}

//处理方法2
void test_func2(void *data)
{
    int *i = (int *)data;

    printf("test_func2 normal_thread i is %d,tid is :%ld\n", *i, pthread_self());
    // sleep(1);
}

int main(int argc, char **argv)
{
    THREADPOOL_S *p_pool = thread_pool_init();
    for (int i = 0; i < 10; i++)
    {
        TASK_NODE_S task;
        if (i % 2 == 0) //偶数，func
        {
            thread_pool_add_task(p_pool, test_func, (void *)&i, 1);
        }
        else    //奇数
        {
            thread_pool_add_task(p_pool, test_func2, (void *)&i, 0);
        }
        // sleep(1);
    }
    // sleep(1000);
    
    return 0;
}
