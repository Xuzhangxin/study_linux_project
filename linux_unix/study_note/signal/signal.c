#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

//当信号发生时，由内核来调用该函数
void sig_fun(int sig)
{
    printf("sig = %d\n", sig);
}

int main(int argc, char **argv)
{
    //进程本身无法感知，所以告知内核，收到该信号时，运行注册的信号服务程序
    signal(SIGINT,sig_fun);//ctrl+c信号
    while (1) {
        printf("main run\n");
        sleep(1);
    }
}

