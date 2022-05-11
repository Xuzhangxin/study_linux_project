#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

//ipcs  查看已有进程通信
int main(int argc, char **argv)
{
    //1.创建共享内存                            已存在则返回ID即可
    int shmid = shmget((key_t)1234, 256, IPC_CREAT|0600);
    assert(shmid != -1);

    //2.映射共享内存
    char *mem = shmat(shmid, NULL, 0);//可读可写
    assert(mem != (char *)-1);
    //3.写入共享内存
    strcpy(mem, "hello");
    //4.断开共享内存
    shmdt(mem);

    exit(0);
}