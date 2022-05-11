#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
//父子进程之间是独立的
//父子进程相当于是并发执行，执行顺序不一定
// int main(int argc, char **argv)
// {
//     pid_t id = fork();
//     char *str = NULL;
//     int n = 0;

//     if (id == 0) {
//         str = "child";
//         n = 2;
//     } else if (id > 0) { //父进
//         str = "parent";
//         n = 7;
//         int status = 0;
//         wait(&status);
//         if (WIFEXITED(status)) {//判断是否为正常退出
//             //状态是0-128一个字节，而status是int
//             //exit为3 status则为768
//             printf("child exit status:%d\n", WEXITSTATUS(status));
//         }   
//     }
//     int i = 0;
//     for (; i < n; i++) {
//         printf("str:%s, pid:%d, ppid:%d.\n", str, getpid(), getppid());
//         sleep(1);
//     }

//     exit(3);
//     //父子进程都会执行
// }



int main(int argc, char **argv)
{

    int fd = open("file.txt",  O_RDONLY);
    assert( fd != -1 );
    
    pid_t id = fork();
    assert( id != -1 );
    if (id == 0) {
        char buf[1024] = {0};
        int n = read(fd, buf, 1);
        printf("child:%s \n", buf);
        sleep(1);
    } else if (id > 0) { //父进
        char buf[1024] = {0};
        int n = read(fd, buf, 1);
        printf("father:%s \n", buf);
        sleep(1);
    }

    exit(0);
    //父子进程都会执行
}

