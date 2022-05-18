#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>

typedef struct queue_node {
    struct queue_node *next;
    void *value;
} QUEUE_NODE_S;

typedef struct queue {
    QUEUE_NODE_S *head;
    QUEUE_NODE_S *tail;
    int bytes;
    int cnt; // 队列长度
    int max_depth; // 最大深度
    pthread_mutex_t mutex; // 内部互斥锁
} QUEUE_S;

typedef void (*QUEUE_DESTROY_CB) (void *data);

QUEUE_S *test_queue_init(int bytes, int max_depth);

void test_in_queue_malloc(QUEUE_S *p_queue, void *value);

void *test_out_queue_malloc(QUEUE_S *p_queue);

void test_queue_destroy(QUEUE_S *p_queue, QUEUE_DESTROY_CB cb);

int test_queue_depth_get(QUEUE_S *p_queue);


#endif