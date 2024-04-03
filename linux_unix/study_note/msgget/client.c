#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define MSGQ_PATH "./"
#define MSGQ_PRI 100

typedef struct {
    long mtype;
    char mtext[256];
} MSGQ_T;

int iMsgId = 0;
pthread_t msgqRecvThread = 0; 

void *msgqRecvThreadTask(void *arg)
{
    while (1)
    {
        MSGQ_T msgBuf = {0};
        int ret = msgrcv(iMsgId, &msgBuf, sizeof(msgBuf), 0, 0);
        if (ret < 0)
        {
            return NULL;
        }

        printf("msgq:%d, recv msgtype:%d, msgtext:%s, ret:%d \n", iMsgId, msgBuf.mtype, msgBuf.mtext, ret);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    key_t msgkey = 0;
    msgkey = ftok(MSGQ_PATH, MSGQ_PRI);
    if (msgkey == -1)
    {
        printf("ftok error, pid:%d\n", getpid());
        perror("....");
        return -1;
    }

    iMsgId = msgget(msgkey, IPC_CREAT | 0644);
    if (iMsgId == -1)
    {
        printf("msgget error, pid:%d\n", getpid());
        perror("....");
        return -1;
    }
    printf("msgget success, pid:%d, key:%d, msgid:%d\n", getpid(), msgkey, iMsgId);

    
    pthread_create(&msgqRecvThread, NULL, msgqRecvThreadTask, NULL);
    pthread_join(msgqRecvThread, NULL);

    return 0;
}