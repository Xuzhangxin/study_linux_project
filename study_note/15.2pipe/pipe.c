#include "apue.h"

#define LINE 1024

int main(int argc, char **argv)
{
    int fd1[2], fd2[2];
    pid_t pid;
    int num;
    char buf[LINE];

    if (pipe(fd1) < 0) {
        err_sys("pipe fd1 error");
    }
    if (pipe(fd2) < 0) {
        err_sys("pipe fd2 error");
    }
    printf("prepare fork\n");
    pid = fork();
    if (pid < 0) {
        err_sys("fork error");
    } else if (pid > 0) {
        printf("prepare fork1\n");
        //父进程：返回子进程ID
        close(fd1[0]);  //管道1，父写子读。 管道2，父读子写
        close(fd2[1]); 
        char *str = "hello, this data is parent write by pipe1\n";
        write(fd1[1], str, strlen(str));

        num = read(fd2[0], buf, LINE);
        write(STDOUT_FILENO, buf, num);
    } else {
        printf("prepare fork2\n");
        //子进程：返回0
        close(fd1[1]);  //管道1，父写子读。 管道2，父读子写
        close(fd2[0]);
        num = read(fd1[0], buf, LINE);
        write(STDOUT_FILENO, buf, num);

        char *str2 = "hello, this data is parent write by pipe2\n";
        write(fd2[1], str2, strlen(str2));
    }
    exit(0);
}