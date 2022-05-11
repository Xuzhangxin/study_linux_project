#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

int main(int argc, char **argv)
{
    //管道只有读或者写的方式
    int fd = open("./fifo", O_WRONLY);
    assert(fd != -1);

    printf("fd = %d\n", fd);

    while (1) {
        printf("please input:\n");
        char buf[128] = {0};
        fgets(buf, sizeof(buf), stdin);
                write(fd, buf, strlen(buf) + 1);
        if (strncmp(buf, "end", 3) == 0) {
            break;
        }
    }
    

    close(fd);
    exit(0);

    return 0;
}