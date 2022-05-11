#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>


//管道写入的数据永远在内存上，所以管道文件大小永远是0
//linux_exec_cmd
//向已经关闭的管道写数据，会sigpipe

//半双工
//为什么无名管道只能在父子进程使用
//通过先创建出管道后，采用fork的方式，使父子进程共享管道
//而如果先fork，他们则访问的不是同一份，无法共享
int main(int argc, char **argv)
{
    int fd[2]; //父子进程共享fork前打开的文件，fd[0] r fd[1] w
    int rt = pipe(fd);//1.创建pipe
    assert( rt != -1 );

    rt = fork();//2.fork父子进程
    assert( rt != -1 );
    if (rt == 0) {
        //子进程读，父进程写
        close(fd[1]);//3.子进程关闭写端
        char buf[1024];
        read(fd[0], buf, sizeof(buf));
        printf("child read:%s\n", buf);
        close(fd[0]);
    } else if (rt > 0) {
        close(fd[0]);//4.父进程关闭读端
        char *buf  = "hello world";
        write(fd[1], buf, strlen(buf) + 1);
        printf("parent write:%s\n", buf);
        close(fd[1]);
    }

    exit(0);
}