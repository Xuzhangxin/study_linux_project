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
int main(int argc, char **argv)
{
    //管道只有读或者写的方式
    int fd = open("./fifo", O_RDONLY);
    assert(fd != -1);
    int n = 0;

    printf("fd = %d\n", fd);

    while(1) {
        char buf[128];
        n = read(fd, buf, 128);
        if (n == 0) {
            break;
        }
        printf("buf is %s", buf);
    }

    close(fd);
    exit(0);

    return 0;
}