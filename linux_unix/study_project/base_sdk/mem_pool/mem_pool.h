#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__

#include <pthread.h>

void mem_pool_init();

char *Malloc(int bytes);

void Free(char *addr);

#endif