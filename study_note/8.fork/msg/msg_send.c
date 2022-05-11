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

    for (int i = 0; i < 5; i++) {
        MSG_S msg_send = {0};
        msg_send.type = i;
        snprintf(msg_send.data, sizeof(msg_send.data), "hello_%d", i);
        ret = msgsnd(msg_id, (void *)&msg_send, 128, 0);
    }



    exit(0);
}