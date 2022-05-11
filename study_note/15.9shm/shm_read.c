#include "apue.h"
#include <sys/ipc.h>
#include <sys/shm.h>
//write hello world 
//another program read this msg

int main(int argc, char **argv)
{
    //1.根据文件和id获取key
    key_t key;
    key = ftok(".", 2);

    //2.根据key创建共享存储区
    int shmid = 0;
    shmid = shmget(key, 1024, 0);
    if (shmid == -1) {
        perror("shmget error");
        return -1;
    }

    //3.连接共享存储区和进程地址空间
    char *shmaddr = NULL;
    shmaddr = shmat(shmid, 0, 0);
    if (shmaddr == (char *)-1) {
        perror("shmat error");
        return -1;
    }

    printf("read data is %s \n", shmaddr);

    //5. 断开共享存储连接
    shmdt(shmaddr);

    printf("success read!!\n");
    return 0;
}