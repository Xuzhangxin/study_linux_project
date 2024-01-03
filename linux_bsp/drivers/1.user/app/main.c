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

    int fd = open("/dev/user_chrdev", O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    printf("open success\n");

    write(fd, w_buf, 10);
    read(fd, r_buf, 10);

    close(fd);


    return 0;
}