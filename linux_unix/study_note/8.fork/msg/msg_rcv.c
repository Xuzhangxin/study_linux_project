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
#include <sys/msg.h>

typedef struct msg{
    long type;
    char data[128];
}MSG_S;

int main(int argc, char **argv)
{
    int ret = 0;

    int msg_id = msgget((key_t)(11), IPC_CREAT|0666);
    assert(msg_id != -1);

    MSG_S msg_recv = {0};
    ret = msgrcv(msg_id, (void *)&msg_recv, 128, 0, 0);

    printf("msg recv :%s, ret:%d", msg_recv.data, ret);

    // msgctl(msg_id, IPC_RMID, NULL);

    exit(0);
}