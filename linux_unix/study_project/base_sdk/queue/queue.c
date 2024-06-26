#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"
#include "base.h"

// 读写锁相比互斥锁而言，有更好的并行性，多个线程只是想共享读数据的时候，不用阻塞等待
// 写操作比较多或本身操作不怎么频繁的程序中，使用互斥量，读操作大于写操作的用读写锁同步

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
    FREE_NODE_CB free_node_cb; 
} QUEUE_S;

/**
 * 函数名：queue初始化
 * 作用：初始化锁等资源
 * 入参：bytes:队列单个节点的数据大小（仅data）
 * 返回值：队列
 */
QUEUE_HANDLE test_queue_init(int bytes, int max_depth, FREE_NODE_CB cb)
{
    int ret = 0;

    QUEUE_S *p_queue = (QUEUE_S *)malloc(sizeof(QUEUE_S));
    CHECK_NULL_RETURN_NULL(p_queue);
    memset(p_queue, 0, sizeof(QUEUE_S));

    ret = pthread_mutex_init(&p_queue->mutex, NULL); //默认锁
    if (ret != 0) {
        MY_PRINTF("queue mutex init failed");
        return NULL;
    }

    p_queue->bytes = bytes;
    p_queue->cnt = 0;
    p_queue->max_depth = max_depth;
    // 队列头和尾，都指向NULL
    p_queue->head = NULL;
    p_queue->tail = NULL;
    p_queue->free_node_cb = cb;
    return p_queue;
}

// 入队函数（数据内存复制给内部）
int test_in_queue_malloc(QUEUE_HANDLE handle, void *value)
{
    CHECK_NULL_RETURN_VAL(handle);
    QUEUE_S *p_queue = (QUEUE_S *)handle;
    printf("in queue cnt: %d, max_depth:%d\n", p_queue->cnt, p_queue->max_depth);
    // CHECK_NULL_RETURN(value);
    if (p_queue->cnt >= p_queue->max_depth) {
        return -1;
    }

    QUEUE_NODE_S *node = (QUEUE_NODE_S *)malloc(sizeof(QUEUE_NODE_S));
    CHECK_NULL_RETURN_VAL(node);
    memset(node, 0, sizeof(QUEUE_NODE_S));   

    node->next = NULL;
    node->value = NULL;

    pthread_mutex_lock(&p_queue->mutex);

    void *p_value = NULL;
    if (value != NULL) {
        p_value = (void *)malloc(p_queue->bytes);
        if (NULL == p_value) {
            pthread_mutex_unlock(&p_queue->mutex);
            MY_PRINTF("in queue malloc failure\n");
            free(node);
            node = NULL;
            return -1;
        }

        memcpy(p_value, value, p_queue->bytes); 
        node->value = p_value;
        MY_PRINTF("in queue value: %p\n", p_value);
    }
  

    // 新节点成为尾节点,原先的尾节点指向新节点
    if (p_queue->tail != NULL) {
        p_queue->tail->next = node;
    }
    
    p_queue->tail = node;
    p_queue->cnt++;
    // 头节点为空，则头尾节点为同一个
    if (p_queue->head == NULL) {
        p_queue->head = p_queue->tail;
    }

    pthread_mutex_unlock(&p_queue->mutex);

    return 0;
}

// 出队函数（数据内存复制给外部）
void *test_out_queue_malloc(QUEUE_HANDLE handle)
{
    CHECK_NULL_RETURN_NULL(handle);
    QUEUE_S *p_queue = (QUEUE_S *)handle;

    pthread_mutex_lock(&p_queue->mutex);

    if (p_queue->cnt <= 0) {
        return NULL;
    }

    // 头节点出队
    QUEUE_NODE_S *node = p_queue->head;
    if (node == NULL) {
        pthread_mutex_unlock(&p_queue->mutex);
        MY_PRINTF("p_queue->head queue is empty\n");
        return NULL;
    }

    p_queue->head = p_queue->head->next;
    if (p_queue->head == NULL) {
        p_queue->tail = p_queue->head;
    }

    void *p_value = NULL;
    // 数据复制给外部
    if (p_queue->bytes != 0) {
        p_value = (void *)malloc(p_queue->bytes);
        CHECK_NULL_RETURN_NULL(p_value);
        memcpy(p_value, node->value, p_queue->bytes);
        MY_PRINTF("out queue value: %p\n", node->value);
    }
    p_queue->cnt--;

    if (p_queue->free_node_cb != NULL) {
        p_queue->free_node_cb(node->value); 
    }

    pthread_mutex_unlock(&p_queue->mutex);

    if (node != NULL) {
        free(node);
        node = NULL;
    }

    return p_value;
}

void test_queue_destroy(QUEUE_HANDLE handle) //destroy 函数给外部CB
{
    CHECK_NULL_RETURN(handle);
    QUEUE_S *p_queue = (QUEUE_S *)handle;

    pthread_mutex_lock(&p_queue->mutex);

    // 头节点出队
    QUEUE_NODE_S *node = NULL;

    while((node = p_queue->head) != NULL) {
        
        if (node != NULL) {
            if (p_queue->free_node_cb != NULL) {
                p_queue->free_node_cb(node->value); 
            }
        }
        p_queue->head = p_queue->head->next;
    }

    p_queue->cnt = 0;
    p_queue->max_depth = 0;
    pthread_mutex_unlock(&p_queue->mutex);

    return;
}

int test_queue_depth_get(QUEUE_HANDLE handle)
{
    CHECK_NULL_RETURN_VAL(handle);
    QUEUE_S *p_queue = (QUEUE_S *)handle;

    return p_queue->cnt;
}

bool test_queue_is_empty(QUEUE_HANDLE handle)
{
    if (handle == NULL) {
        return true;
    }
    QUEUE_S *p_queue = (QUEUE_S *)handle;

    return !(p_queue->cnt);
}

// typedef struct test_struct {
//     int value_int;
// } TEST_STRUCT_S;


// void free_node_cb(void *data)
// {
//     free(data); //没有指向NULL
//     printf("free data:%p\n", data);
// }

// int main(int argc, char **argv)
// {
//     QUEUE_S *my_queue = test_queue_init(sizeof(TEST_STRUCT_S), 100, free_node_cb);

//     for (int i = 0; i < 20; i++) {
//         TEST_STRUCT_S value = {0};
//         value.value_int = i;
//         test_in_queue_malloc(my_queue, (void *)&value);
//     }

//     printf("my_queue cnt is:%d\n", my_queue->cnt);
//     fflush(stdout);

//     for (int i = 0; i < 5; i++) {
//         TEST_STRUCT_S *p_value = (TEST_STRUCT_S *)test_out_queue_malloc(my_queue);
//         printf("out p_value int2 is:%d\n", p_value->value_int);
//         fflush(stdout);
//     }

//     // bool if_empty = test_queue_is_empty(my_queue);
//     // printf("if_empty :%d\n", if_empty);

//     //  test_queue_destroy(my_queue);
    
//     // // printf("my_queue cnt is:%d\n", my_queue->cnt);

//     // if_empty = test_queue_is_empty(my_queue);
//     // printf("if_empty :%d\n", if_empty);

//     return 0;
// }