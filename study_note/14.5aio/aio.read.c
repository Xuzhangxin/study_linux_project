#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <aio.h>
//gcc aio.read.c -lrt
void p_time(char *tip)
{
    struct timeb tb;
    ftime(&tb);
    fprintf(stdout, "%s %lu : %u\n", tip, tb.time, tb.millitm);
}

int main(int argc, char **argv)
{
    int fd = -1, ret = -1;

    fd = open("./file.txt", O_RDONLY);
    if (fd < 0) {
        perror("open file.txt");
        return -1;
    }

    struct aiocb read_cb = {0};
    read_cb.aio_fildes = fd;    /* File desriptor.  */
    read_cb.aio_reqprio = 0;    /* Request priority offset.  */
    read_cb.aio_nbytes = 1024*1024; /* Length of transfer.  */

    char buf[1024*1024 + 1] = {0};
    read_cb.aio_buf = buf;

    p_time("start read");

    ret = aio_read(&read_cb);
    if (ret < 0) {
        perror("aio_read");
        return -2;
    }

    p_time("reading");

    while(aio_error(&read_cb) == EINPROGRESS);

    p_time("after read");

    if ((ret = aio_return(&read_cb)) > 0) {
        fprintf(stdout, "read:%s\n", read_cb.aio_buf);
    } else {
        fprintf(stdout, "return :%d\n", ret);
    }

    close(fd);

}

//https://www.cnblogs.com/schips/p/12575933.html