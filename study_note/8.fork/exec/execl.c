#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

int main(int argc, char **argv)
{
    printf("main pid=%d, ppid = %d\n", getpid(), getppid());
    pid_t pid = fork();
    assert(pid != -1);

    if (pid == 0) {
        printf("child pid=%d, ppid = %d\n", getpid(), getppid());
        //用Ps将子进程替代
        //ps是名字，作为启动程序的第一个参数。不传这个名字也行
        execl("/bin/ps", "ps", "-aux", NULL);//NULL代表结束
        //如果替代失败，才由可能执行到这
        printf("execl error:");
    } else {
        wait(NULL);
    }
    exit(0);

    return 0;
}