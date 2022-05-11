#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

// struct MSG_S {
//     struct MSG_S *next;
//     int num;
//     //more stuff here 
// };

// struct MSG_S *workq = NULL;

// pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
// pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

// void process_msg(void)
// {
//     struct MSG_S *mp = NULL;

//     for (;;) {
//         pthread_mutex_lock(&g_lock);
//             while (workq == NULL) {
//                 pthread_cond_wait(&g_cond, &g_lock);
//             }
//             mp = workq;
//             workq = mp->next;
//             pthread_mutex_unlock(&g_lock);
//             /* now process the message mp*/
//     }
// }


// void enqueue_msg(struct MSG_S *mp)
// {
//     pthread_mutex_lock(&g_lock);

//     mp->next = workq;
//     workq = mp;

//     pthread_mutex_unlock(&g_lock);
//     pthread_cond_signal(&g_cond);
// }

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

/**
 * pthread_exit不可返回局部变量
 * 1) 入参作为返回值的方式
 * 2) malloc内存，再通过清理函数释放
 * 3）static 变量
 */

typedef void *(*thread_func)(void *);
pthread_t thread_create_and_start(thread_func func)
{
    pthread_t id = 0;

    pthread_attr_t attr = {0};
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 512);//栈512
    pthread_attr_setschedpolicy(&attr, SCHED_RR); //实时 时间片轮转

    int policy = 0;
    pthread_attr_getschedpolicy(&attr, &policy);
    printf("pthread policy is:%d\n", policy);

    sched_param param = {0};
    param.sched_priority = 20;
    pthread_attr_setschedparam(&attr, &param);
    sched_param param_out = {0};
    pthread_attr_getschedparam(&attr, &param_out);
    printf("pthread sched_param is:%d\n", param_out.sched_priority);

    int rt = pthread_create(&id, &attr, func, NULL);
    assert(rt != -1);

}

void *thread1_func(void *arg)
{
    pthread_mutex_lock(&g_lock);

    pthread_cond_wait(&g_cond, &g_lock);
    printf("thread1_func\n");
    sleep(2);
    printf("thread1_func\n");
    pthread_mutex_unlock(&g_lock);


    pthread_exit((void *)(-2));
}

void *thread2_func(void *arg)
{
    pthread_mutex_lock(&g_lock);

    pthread_cond_wait(&g_cond, &g_lock);//会先释放锁，等待到条件变量后，再拿锁，主要是保护g_cond
    printf("thread2_func\n");
    sleep(2);
    printf("thread2_func\n");

    pthread_mutex_unlock(&g_lock);

    pthread_exit(NULL);
}
void *thread3_func(void *arg)
{
    printf("thread3_func\n");
    sleep(2);
    printf("thread3_func\n");

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{

    pthread_mutexattr_t attr = {0};
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL); //普通锁
    pthread_mutex_init(&g_lock, &attr);

    pthread_cond_init(&g_cond, NULL);

    int rt = 0;
    pthread_t tid_1 = 0;
    pthread_t tid_2 = 0;

    rt = pthread_create(&tid_1, NULL, thread1_func, NULL);
    assert(rt != -1);

    rt = pthread_create(&tid_2, NULL, thread2_func, NULL);
    assert(rt != -1);

    thread_create_and_start(thread3_func);

    sleep(2);
    pthread_cond_signal(&g_cond);
    sleep(2);
    pthread_cond_signal(&g_cond);

    int *ret_t = NULL;
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    // printf("pthread_exit ret:%d", *(int *)ret_t);

    exit(0);
    // timespec nowtime = {0x0};
    // clock_gettime(CLOCK_REALTIME, &nowtime);
    // printf("sec is %ld", nowtime.tv_sec);
}