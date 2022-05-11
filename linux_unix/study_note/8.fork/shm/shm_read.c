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
    //1.获取共享内存                            已存在则返回ID即可
    int shmid = shmget((key_t)1234, 256, IPC_CREAT|0600);
    assert(shmid != -1);

    //2.映射共享内存
    char *mem = shmat(shmid, NULL, 0);//可读可写
    assert(mem != (char *)-1);
    //3.读共享内存
    printf("data is :%s\n", mem);
    //4.断开共享内存
    shmdt(mem);
    //删除共享内存
    shmctl(shmid , IPC_RMID, NULL);
    exit(0);
}