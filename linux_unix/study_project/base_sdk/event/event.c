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

#define EVENT_NAME_LEN 64



typedef void* EVENT_HANDLE;
typedef void (*EVENT_FUNC_CB) (void *arg);

typedef struct timer_list_node_s {
    LINK_LIST_S list;
    EVENT_FUNC_CB cb;
    char event_name[EVENT_NAME_LEN];
} EVENT_LIST_NODE_S;

typedef struct base_timer_s {
    EVENT_LIST_NODE_S mgr_list; //
} BASE_EVENT_S;

static BASE_EVENT_S *g_mgr = NULL;

int base_event_init();


int base_event_init()
{
    int ret = 0;

    g_mgr = malloc(sizeof(BASE_EVENT_S));
    CHECK_NULL_RETURN_VAL(g_mgr);
    memset(g_mgr, 0, sizeof(BASE_EVENT_S));

    link_list_init(&g_mgr->mgr_list.list);
    g_mgr->mgr_list.cb = NULL;

    return 0;
}

// 取消订阅事件
void base_event_unsubcribe(char *name)
{
    CHECK_NULL_RETURN(name);
    EVENT_LIST_NODE_S *data = NULL;
    link_list_for_each_entry(&g_mgr->mgr_list.list, EVENT_LIST_NODE_S, list, data) {
        if (strcmp(data->event_name, name) == 0) {
            // 取消订阅,删除节点
            link_list_del_node(&g_mgr->mgr_list.list, &data->list);
            free(data);
        }
    }    
}

static void list_node_free_cb(void *data)
{
    CHECK_NULL_RETURN(data);
    EVENT_LIST_NODE_S *event = (EVENT_LIST_NODE_S *)data; 
    // printf("handle:%p, free, success\n", timer);
    
    free(event);
}

// 取消所有订阅事件
void base_event_unsubcribe_all()
{
    link_list_destroy(&g_mgr->mgr_list.list, list_node_free_cb, EVENT_LIST_NODE_S, list);  
}

// 销毁事件回调机                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     制
void base_event_destroy()
{
    // 其中不包括头节点
    base_event_unsubcribe_all(); 

    free(g_mgr); // 释放头节点及全局内存
    g_mgr = NULL;
}

// 订阅事件
void base_event_subcribe(EVENT_FUNC_CB cb, char *name)
{
    CHECK_NULL_RETURN(cb);
    EVENT_LIST_NODE_S *event = (EVENT_LIST_NODE_S *)malloc(sizeof(EVENT_LIST_NODE_S));
    CHECK_NULL_RETURN(event);
    memset(event, 0, sizeof(EVENT_LIST_NODE_S));

    event->cb = cb; //回调函数
    strncpy(event->event_name, name, EVENT_NAME_LEN);

    link_list_add_tail_node(&g_mgr->mgr_list.list, &event->list);

    return ;
}

// 推送同步事件 
void base_event_publish(char *name, void *arg)
{
    CHECK_NULL_RETURN(name);
    EVENT_LIST_NODE_S *data = NULL;
    link_list_for_each_entry(&g_mgr->mgr_list.list, EVENT_LIST_NODE_S, list, data) {
        if (strcmp(data->event_name, name) == 0) {
            data->cb(arg);
        }
    }
    return ;
}

#define EVENT_TASK_1 "task1"

void cb_func(void *arg)
{
    int *data = (int *)arg;

    printf("cb_func is :%d", *data);
}

int main(int argc, char ** argv)
{
    base_event_init();

    int arg = 1000;

    EVENT_HANDLE handle = NULL;

    base_event_subcribe(cb_func, EVENT_TASK_1);

    base_event_publish(EVENT_TASK_1, &arg);

    return 0;
}



