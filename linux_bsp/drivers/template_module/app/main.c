#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"



int main(int argc, char **argv)
{
    // char r_buf[3] = {0};
    // int count = 100;
    // int fd = open("/dev/ap3216c", O_RDWR);
    // if (fd < 0)
    // {
    //     printf("open ap3216c failed\n");
    //     return -1;
    // }
    // printf("open ap3216c success\n");
    // while (count--) {
    //     read(fd, r_buf, sizeof(r_buf) / sizeof(r_buf[0]));
    //     printf("read ap3216c ir:%d\n", r_buf[0]);
    //     printf("read ap3216c als:%d\n", r_buf[1]);
    //     printf("read ap3216c ps:%d\n", r_buf[2]);
    //     usleep(200 * 1000);
    // }
    // close(fd);


    return 0;
}