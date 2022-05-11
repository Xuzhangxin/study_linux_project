#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "base.h"

// 读写锁相比互斥锁而言，有更好的并行性，多个线程只是想共享读数据的时候，不用阻塞等待
// 写操作比较多或本身操作不怎么频繁的程序中，使用互斥量，读操作大于写操作的用读写锁同步

// 链式队列实现

// 入操作用size来操作是否更好

/**
 * 函数名：queue初始化
 * 作用：初始化锁等资源
 * 入参：bytes:队列单个节点的数据大小（仅data）
 * 返回值：队列
 */
QUEUE_S *test_queue_init(int bytes)
{
    int ret = 0;

    QUEUE_S *p_queue = (QUEUE_S *)malloc(sizeof(QUEUE_S));
    CHECK_NULL_RETURN_NULL(p_queue);
    memset(p_queue, 0, sizeof(QUEUE_S));

    ret = pthread_mutex_init(&p_queue->mutex, NULL); //默认锁
    if (ret != 0) {
        printf("queue mutex init failed");
        return NULL;
    }

    p_queue->bytes = bytes;
    p_queue->cnt = 0;
    // 队列头和尾，都指向NULL
    p_queue->head = NULL;
    p_queue->tail = NULL;

    return p_queue;
}

// 入队函数（数据内存复制给内部）
void test_in_queue_malloc(QUEUE_S *p_queue, void *value)
{
    CHECK_NULL_RETURN(p_queue);
    // CHECK_NULL_RETURN(value);


    QUEUE_NODE_S *node = (QUEUE_NODE_S *)malloc(sizeof(QUEUE_NODE_S));
    CHECK_NULL_RETURN(node);
    memset(node, 0, sizeof(QUEUE_NODE_S));   

    node->next = NULL;
    node->value = NULL;

    pthread_mutex_lock(&p_queue->mutex);

    void *p_value = NULL;
    if (value != NULL) {
        p_value = (void *)malloc(p_queue->bytes);
        if (NULL == p_value) {
            pthread_mutex_unlock(&p_queue->mutex);
            printf("in queue malloc failure\n");
            free(node);
            node = NULL;
            return;
        }

        memcpy(p_value, value, p_queue->bytes); 
    }
  

    // 新节点成为尾节点,原先的尾节点指向新节点
    if (p_queue->tail != NULL) {
        p_queue->tail->next = node;
    }
    
    p_queue->tail = node;
    p_queue->tail->value = p_value;
    p_queue->cnt++;
    // 头节点也为空，则头尾节点为同一个
    if (p_queue->head == NULL) {
        p_queue->head = p_queue->tail;
    }

    pthread_mutex_unlock(&p_queue->mutex);

    return;
}

// 出队函数（数据内存复制给外部）
void *test_out_queue_malloc(QUEUE_S *p_queue)
{
    CHECK_NULL_RETURN_NULL(p_queue);

    pthread_mutex_lock(&p_queue->mutex);

    if (p_queue->cnt <= 0) {
        return NULL;
    }

    // 头节点出队
    QUEUE_NODE_S *node = p_queue->head;
    if (node == NULL) {
        pthread_mutex_unlock(&p_queue->mutex);
        printf("p_queue->head queue is empty\n");
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
    }
    p_queue->cnt--;

    pthread_mutex_unlock(&p_queue->mutex);

    if (node->value != NULL) {
        free(node->value);
        node->value = NULL;    
    }

    if (node != NULL) {
        free(node);
        node = NULL;
    }

    return p_value;
}

void test_queue_destroy(QUEUE_S *p_queue)
{
    CHECK_NULL_RETURN(p_queue);

    pthread_mutex_lock(&p_queue->mutex);

    // 头节点出队
    QUEUE_NODE_S *node = NULL;

    // 循环出队，直到队列头节点为NULL
    while((node = p_queue->head) != NULL) {
        if (node->value != NULL) {
            free(node->value);
            node->value = NULL;    
        }

        if (node != NULL) {
            free(node);
            node = NULL;
        }
        p_queue->head = p_queue->head->next;
    }

    p_queue->cnt = 0;
    pthread_mutex_unlock(&p_queue->mutex);

    return;
}


// typedef struct test_struct {
//     int value_int;
//     int value_arr[5];
// } TEST_STRUCT_S;

// int main(int argc, char **argv)
// {
//     TEST_STRUCT_S value = {-1};
//     memset(&value, -1, sizeof(TEST_STRUCT_S));

//     QUEUE_S *my_queue = test_queue_init(sizeof(TEST_STRUCT_S));

//     for (int i = 0; i < 10; i++) {
//         TEST_STRUCT_S value = {0};
//         value.value_int = i;
//         for (int j = 0; j < 5; j++) {
//             value.value_arr[j] = i;
//         }
//         printf("p_value int is:%d\n", value.value_int);
//         fflush(stdout);
//         test_in_queue_malloc(my_queue, (void *)&value);
//     }

//     printf("my_queue int is:%d\n", my_queue->cnt);
//     fflush(stdout);
//     for (int i = 0; i < 10; i++) {
//         TEST_STRUCT_S *p_value = (TEST_STRUCT_S *)test_out_queue_malloc(my_queue);
//         printf("p_value int is:%d\n", p_value->value_int);
//         fflush(stdout);
//         for (int j = 0; j < 5; j++) {
//             printf("p_value arr[%d] is:%d\n", j, p_value->value_arr[j]);
//             fflush(stdout);
//         }
//     }

//     return 0;
// }