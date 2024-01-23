#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"



int main(int argc, char **argv)
{
    char w_buf[1024] = {0};
    char r_buf[1024] = {0};

    int fd = open("/dev/dtsled", O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    printf("open success\n");

    snprintf(w_buf, sizeof(w_buf) / sizeof(w_buf[10]), "hello world");

    write(fd, w_buf, 10);
    read(fd, r_buf, 10);

    printf("r_buf :%s", r_buf);

    close(fd);


    return 0;
}