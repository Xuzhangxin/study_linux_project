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

static int semid = -1;

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void sem_init();
void sem_p();
void sem_v();
void sem_destroy();

//有名信号量
//sem_ctl 某些flag是针对单个成员的，通过编号
//有些则是针对整个信号量集
void sem_init()
{
    int rt = 0;
    //谁先来谁创建，后来的报错
    semid = semget((key_t)(1234), 1, IPC_CREAT | IPC_EXCL | 0600);
    if(semid == -1) {
        printf("semkey:key_t is exist, so use\n");
        semid = semget((key_t)(1234), 1, 0600);
        if(semid == -1) {
            printf("semkey: get errro\n");
            return;
        }
    } else {
        union semun sem_u = {0};
        sem_u.val = 1;
        rt = semctl(semid, 0, SETVAL, sem_u);//对整个集合进行操作，也可指定第几个
        if (rt == -1) {
           perror("semctl error"); 
           return;
        }
    }

    return;
}

//减一
void sem_p()
{
    struct sembuf buf = {0};
    buf.sem_flg = SEM_UNDO; 
    //在修改信号量值时，若进程正常或异常退出时，操作系统将信号量归还
    buf.sem_num = 0;//第0个信号量
    buf.sem_op = -1;//val = val +(-1)

    if (semop(semid, &buf, 1) == -1) {
        perror("sem_p error");
    }
}

//+1
void sem_v()
{
    struct sembuf buf = {0};
    buf.sem_flg = SEM_UNDO; 
    //在修改信号量值时，若进程正常或异常退出时，操作系统将信号量归还
    buf.sem_num = 0;//第0个信号量
    buf.sem_op = 1;//val = val +(-1)

    if (semop(semid, &buf, 1) == -1) {
        perror("sem_v error");
    }
}

void sem_destroy()
{
    //移除整个集合，
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("sem_destroy error");
    }
}

int main(int argc, char **argv)
{
    sem_init();//fork前创建，父子进程公用一个
    int pid = fork();
    assert(pid != -1);


    if (pid == 0) {
        int n = 3;
        for (int i = 0; i < n; i++) {
            sem_p();
            printf("A\n");
            sleep(1);
            printf("A\n");
            sem_v();
        }
    } else if (pid > 0) {
        int n = 10;
        for (int i = 0; i < n; i++) {
            sem_p();
            printf("B\n");
            sleep(1);
            printf("B\n");
            sem_v();
        }
        sem_destroy();
    }

    exit(0);
}