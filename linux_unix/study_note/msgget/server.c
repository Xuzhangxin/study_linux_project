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

int main(int argc, char **argv)
{
    key_t msgkey = 0;
    msgkey = ftok(MSGQ_PATH, MSGQ_PRI);
    if (msgkey == -1)
    {
        printf("ftok error, pid:%d\n", getpid());
        return -1;
    }

    iMsgId = msgget(msgkey, IPC_CREAT | 0644);
    if (iMsgId == -1)
    {
        printf("msgget error, pid:%d\n", getpid());
        return -1;
    }
    printf("msgget success, pid:%d, key:%d, msgid:%d\n", getpid(), msgkey, iMsgId);
    
    while (1)
    {
        MSGQ_T msgBuf = {0};

        int iMsgType = 0;
        printf("input message type:\n");//输入消息类型
        scanf("%d", &iMsgType);
        if (iMsgType == 0)
        {
            printf("message type 0, break\n");
            break;
        }
    
        char acBuf[256] = {0};
        printf("input message to be send:\n");//输入消息信息
        scanf("%s", acBuf);

        msgBuf.mtype = iMsgType;
        strncpy(msgBuf.mtext, acBuf, sizeof(msgBuf.mtext) - 1);

        msgsnd(iMsgId, &msgBuf, sizeof(msgBuf), IPC_NOWAIT);

        printf("msgq:%d, send msgtype:%d, msgtext:%s \n", iMsgId, msgBuf.mtype, msgBuf.mtext);
    }

    msgctl(iMsgId, IPC_RMID, NULL);

    return 0;
}