#include <stdio.h>
#include <string.h>
#include "shm_server.h"

#define     SHM_MGR_MAX    32

typedef struct shm_mgr
{
    SHM_ID shm_id;
    void *shm_addr;
};

typedef struct shm_

shm_mgr g_mgr[SHM_MGR_MAX] = {0}

SHM_ID shm_server_init(const char *name, int size, int flag)
{
    SHM_ID shm_id = -1;

    key_t key = ftok(name, 123);
    if (key < 0)
    {
        printf("shm server init failed, key is error\n");
        return -1;
    }

    shm_id = shmget(key, size, flag);
    if (shm_id < 0)
    {
        printf("shm server init failed, shmget is error\n");
        return -1;
    }

    shmat(shm_id, NULL, 0);

    

    return shm_id;
}

int shm_server_write(SHM_ID id, char *buf, int size)
{
    if (id < 0)
    {
        printf("shm server write failed, id:%d\n", id);
        return -1;   
    }

    
}

int shm_server_write_async(SHM_ID id, char *buf, int size);
int shm_server_destroy(SHM_ID id);
