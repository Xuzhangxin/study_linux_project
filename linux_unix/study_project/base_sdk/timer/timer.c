#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../list/list.h"
#include "base.h"
#include <time.h>

typedef void* TIMER_HANDLE;
typedef unsigned long long TIMER_UNIT;
typedef void (*TIMER_FUNC_CB) (void);

typedef struct timer_list_node_s {
    LINK_LIST_S list;
    TIMER_FUNC_CB cb;
    TIMER_UNIT triggle_time;   // 触发时间->timespec对应
    TIMER_UNIT cycle_ms;
    bool is_running;    // 运行标志
} TIMER_LIST_NODE_S;

typedef struct base_timer_s {
    TIMER_LIST_NODE_S mgr_list; //
    pthread_t tid;
} BASE_TIMER_S;

static BASE_TIMER_S *g_mgr = NULL;

int base_timer_init();

// 创建定时器(cb)
TIMER_HANDLE base_timer_create(TIMER_FUNC_CB cb);

// 定时任务开始
void base_timer_start(TIMER_HANDLE handle, TIMER_UNIT ms);

// 定时任务停止
void base_timer_stop(TIMER_HANDLE handle);

// 定时器销毁
void base_timer_destroy();

// ms级时间
static TIMER_UNIT __get_system_ms_time(void)
{
    TIMER_UNIT ms_time = 0;

    struct timespec time = {0};
    clock_gettime(CLOCK_MONOTONIC, &time); //系统启动开始的时间，稳定。CLOCK_REALTIME 1970年开始的时间 CLOCK_PROCESS_CPUTIME_ID，那么获取的时间表示本进程到当前系统CPU花费的时间

    ms_time = (time.tv_sec*1000) + (time.tv_nsec/1000000);
    return ms_time;
}

void *timer_mgr_func(void *data)
{

    while(1) {
        TIMER_LIST_NODE_S *handle = NULL;
        TIMER_UNIT time = 0;
        // 忽略链表头节点
        link_list_for_each_entry(&g_mgr->mgr_list.list, TIMER_LIST_NODE_S, list, handle) {
            time = __get_system_ms_time();
            if ((handle->triggle_time != 0) && (handle->triggle_time <= time)) { //到达触发时间
                printf("timer triggle! handle:%p, tirggle_time:%llu, now:%llu, cycle_ms:%llu\n", handle, handle->triggle_time, time, handle->cycle_ms);
                handle->cb();
                handle->triggle_time += handle->cycle_ms;
            }
        }
    }
    return 0;
}

int base_timer_init()
{
    int ret = 0;

    g_mgr = malloc(sizeof(BASE_TIMER_S));
    CHECK_NULL_RETURN_VAL(g_mgr);
    memset(g_mgr, 0, sizeof(BASE_TIMER_S));
    printf("malloc g_mgr:%p\n", g_mgr);
    pthread_attr_t attr = {0};
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //分离模式
    pthread_attr_setschedpolicy(&attr, SCHED_RR);// 实时时间片轮转调度
    ret = pthread_create(&g_mgr->tid, &attr, timer_mgr_func, NULL);
    if (ret != 0) {
        return -1;
    }
    link_list_init(&g_mgr->mgr_list.list);
    g_mgr->mgr_list.is_running = false;
    g_mgr->mgr_list.triggle_time = 0;
    g_mgr->mgr_list.cycle_ms = 0;

    return 0;
}

// 创建定时器(cb)
TIMER_HANDLE base_timer_create(TIMER_FUNC_CB cb)
{
    CHECK_NULL_RETURN_NULL(cb);
    TIMER_LIST_NODE_S *timer = (TIMER_LIST_NODE_S *)malloc(sizeof(TIMER_LIST_NODE_S));
    CHECK_NULL_RETURN_NULL(timer);
    memset(timer, 0, sizeof(TIMER_LIST_NODE_S));

    timer->cb = cb; //回调函数
    timer->is_running  = false;
    timer->triggle_time = 0;
    g_mgr->mgr_list.cycle_ms = 0;

    link_list_add_tail_node(&g_mgr->mgr_list.list, &timer->list);

    return (TIMER_HANDLE)timer;
}

// 定时任务开始
void base_timer_start(TIMER_HANDLE handle, TIMER_UNIT ms)
{
    CHECK_NULL_RETURN(handle);
    TIMER_LIST_NODE_S *timer = (TIMER_LIST_NODE_S *)handle;

    timer->triggle_time = __get_system_ms_time() + ms;
    printf("handle :%p, triggle time:%llu\n", handle, timer->triggle_time);

    timer->cycle_ms = ms;
    timer->is_running = true;

    return ;
}

// 定时任务停止
void base_timer_stop(TIMER_HANDLE handle)
{
    CHECK_NULL_RETURN(handle);
    TIMER_LIST_NODE_S *timer = (TIMER_LIST_NODE_S *)handle;

    timer->is_running = false;
    return;
}

// 定时器销毁
void base_timer_delete(TIMER_HANDLE handle)
{
    CHECK_NULL_RETURN(handle);
    TIMER_LIST_NODE_S *timer = (TIMER_LIST_NODE_S *)handle;   

    link_list_del_node(&g_mgr->mgr_list.list, &timer->list);

    printf("base_timer_delete free timer:%p\n", timer);
    free(timer);
    return ;
}

static void list_node_free_cb(void *data)
{
    CHECK_NULL_RETURN(data);
    TIMER_LIST_NODE_S *timer = (TIMER_LIST_NODE_S *)data; 
    printf("handle:%p, free, success\n", timer);
    
    free(timer);
}

void base_timer_destroy()
{
    // 其中不包括头节点
    link_list_destroy(&g_mgr->mgr_list.list, list_node_free_cb, TIMER_LIST_NODE_S, list); 
    printf("free g_mgr:%p\n", g_mgr);
    free(g_mgr); // 释放头节点及全局内存
    g_mgr = NULL;
}

void task_func(void)
{

    // printf("task func\n");
}

int main(int argc, char ** argv)
{
    TIMER_HANDLE tasks[10] = {0};
    int idx = 0;
    base_timer_init();
    int i = 0;
    for (i = 0; i < 10; i++) {
        tasks[i] = base_timer_create(task_func);
        printf("create handle successfully:%p\n", tasks[i]);
    }
    for (i = 0; i < 5; i++) { 
        base_timer_start(tasks[i], (i + 1) * 1000);
        printf("tasks[%d] handle:%p, sleep:%u ms, now:%llu\n", i, tasks[i], i * 1000, __get_system_ms_time());
    }

    sleep(3);
    printf("delete idx:%d timer\n", idx);
    base_timer_delete(tasks[idx]);
    sleep(3);
    base_timer_destroy();

    return 0;
}



