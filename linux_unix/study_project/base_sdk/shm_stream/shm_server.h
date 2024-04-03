#ifndef __SHM_SERVER_H__
#define __SHM_SERVER_H__

#include <sys/ipc.h>
#include <sys/shm.h>

typedef SHM_ID int;

SHM_ID shm_server_init(const char *name, int size, int flag);
int shm_server_write(SHM_ID id, char *buf, int size);
int shm_server_write_async(SHM_ID id, char *buf, int size);
int shm_server_destroy(SHM_ID id);

#endif