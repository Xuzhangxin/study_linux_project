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

typedef struct mem_pool_malloc_table_s {
    int size;
    int num;
} MEM_POOL_MALLOC_TABLE_S;

typedef struct mem_pool_node_s {
    LINK_LIST_S list;
    char *addr;
    int size;
    bool is_using;
} MEM_POOL_NODE_S;

// 链表 管理 16 32 bytes
typedef struct mem_pool_s {
    pthread_mutex_t mutex;
    MEM_POOL_NODE_S node_list;
    MEM_POOL_MALLOC_TABLE_S *table;
    int table_num;
} MEM_POOL_S;

static MEM_POOL_MALLOC_TABLE_S g_table[] = {
    {.num = 10, .size = 32},    // 内存块大小
    {.num = 20, .size = 64},
};

static MEM_POOL_S g_mgr = {0};

static void __mem_pool_data_init(void) 
{
    pthread_mutex_init(&g_mgr.mutex, NULL);

    pthread_mutex_lock(&g_mgr.mutex);

    g_mgr.table =(MEM_POOL_MALLOC_TABLE_S *)&g_table;
    g_mgr.table_num = sizeof(g_table)/sizeof(g_table[0]);

    link_list_init(&g_mgr.node_list.list);

    pthread_mutex_unlock(&g_mgr.mutex);

    printf("__mem_pool_data_init success\n");
    return;
}

static int __mem_pool_malloc(void)
{
    int i = 0, j = 0;
    void *p_mem = NULL;
    int offset = 0;
    MEM_POOL_MALLOC_TABLE_S *table = NULL;

    pthread_mutex_lock(&g_mgr.mutex);

    for (i = 0; i < g_mgr.table_num; i++) {
        table = &g_mgr.table[i];
        p_mem = malloc(table->size * table->num); // 申请大块的连续内存
        if (NULL == p_mem) {
            pthread_mutex_unlock(&g_mgr.mutex);
            return  -1;
        }
        memset(p_mem, 0, table->size * table->num);

        // 记录内存起始位置以及大小
        offset = 0;
        for (j = 0; j < table->num; j++) {
            MEM_POOL_NODE_S *node = malloc(sizeof(MEM_POOL_NODE_S));
            if (NULL == node) {
                pthread_mutex_unlock(&g_mgr.mutex);
                return  -1;
            }
            memset(node, 0, sizeof(MEM_POOL_NODE_S));
            node->addr = p_mem + offset;    // 逐个偏移
            node->size = table->size;   // 内存块大小为size
            node->is_using = false;
            offset += table->size;  // 偏移累加
            link_list_add_tail_node(&g_mgr.node_list.list, &(node->list));
            printf(" mem_pool_malloc list add success, addr=%p, size=%d\n", node->addr, node->size);
        }
    }

    pthread_mutex_unlock(&g_mgr.mutex);

    return 0;

}

void mem_pool_init()
{
    __mem_pool_data_init();
    __mem_pool_malloc();

    return;
}

// note 还可做内存管理
char *Malloc(int bytes) 
{   
    pthread_mutex_lock(&g_mgr.mutex);
//link_list_for_each_entry(list, type, element)
    MEM_POOL_NODE_S *data = NULL;
    link_list_for_each_entry(&g_mgr.node_list.list, MEM_POOL_NODE_S, list, data) {
        if ((data->is_using == false) && (data->size > bytes)) {
            // 选中该内存块
            data->is_using = true;
            printf(" mem_pool_malloc Malloc success, addr=%p, size=%d\n", data->addr, data->size);
            pthread_mutex_unlock(&g_mgr.mutex);
            return data->addr;
        }
    }

    pthread_mutex_unlock(&g_mgr.mutex);
    printf(" mem_pool_malloc Malloc failed, bytes:%d", bytes);

    return NULL;
}


void Free(char *addr)
{
    pthread_mutex_lock(&g_mgr.mutex);
    MEM_POOL_NODE_S *data = NULL;
    link_list_for_each_entry(&g_mgr.node_list.list, MEM_POOL_NODE_S, list, data) {
        if ((data->is_using == true) && (addr == data->addr)) {
            // 选中该内存块
            data->is_using = false;
            printf(" mem_pool_malloc Free success, addr=%p, size=%d\n", data->addr, data->size);
            pthread_mutex_unlock(&g_mgr.mutex);
            return;
        }
    }

    pthread_mutex_unlock(&g_mgr.mutex);
    printf(" mem_pool_malloc Free failed, bytes:%p\n", addr);
    return;
}


int main(int argc, char ** argv)
{
    mem_pool_init();
    char *addr[20];
    int i = 0;
    for (i = 0; i < 20; i++) {
       addr[i] = Malloc(10);  
    }
    for (i = 0; i < 20; i++) {
      Free(addr[i]);  
    }
}



