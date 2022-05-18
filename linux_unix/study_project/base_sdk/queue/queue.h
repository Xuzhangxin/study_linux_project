#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>


typedef void* QUEUE_HANDLE;

typedef void (*FREE_NODE_CB) (void *data);


QUEUE_HANDLE test_queue_init(int bytes, int max_depth, FREE_NODE_CB cb);

void test_in_queue_malloc(QUEUE_HANDLE handle, void *value);

void *test_out_queue_malloc(QUEUE_HANDLE handle);

void test_queue_destroy(QUEUE_HANDLE handle); 

int test_queue_depth_get(QUEUE_HANDLE handle);


#endif