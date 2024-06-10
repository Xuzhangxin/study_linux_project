#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>

#define SEM_NAME "sem_test"
int main(int argc, char **argv)
{
    int iRet = 0;
    int iSemCnt = 0;
    sem_t *pSem = sem_open(SEM_NAME, O_CREAT|O_EXCL, 0777, 1);
    if (!pSem)
    {
        printf("server sem open failed\n");
        return -1;
    }
    sem_getvalue(pSem, &iSemCnt);
    printf("server begin for wait sem:%p, sem cnt:%d\n", pSem, iSemCnt);
    sem_wait(pSem);
    sem_getvalue(pSem, &iSemCnt);
    printf("server wait sem:%p success, sem cnt:%d\n", pSem, iSemCnt);

    sleep(5);
    sem_post(pSem);
    sem_getvalue(pSem, &iSemCnt);
    printf("server send sem:%p success, sem cnt:%d\n", pSem, iSemCnt);

    sleep(5);
    sem_close(pSem);
    sem_unlink(SEM_NAME);

    return 0;
}