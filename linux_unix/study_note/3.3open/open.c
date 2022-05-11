// 使用open实现touch
//生成文件的权限=mode&(~umask)
//write read open close lseek
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"

int main(int argc, char **argv[])
{
    char write_buf[] = "hello world";
    char read_buf[1024] = {0};
    int ret = 0;
    int fd = 0;
    fd = open(argv[1], O_CREAT | O_RDWR | O_EXCL, 0777);
    if (fd < 0) {
        printf("Error opening\n");
        return -1;
    }
    printf("open succeeded, fd = %d\n", fd);

    ret = write(fd, write_buf, sizeof(write_buf));
    if (ret < 0) {
        printf("Error writing\n");
        return -1;
    }
    printf("write succeeded, ret = %d\n", ret);

    lseek(fd, 0, SEEK_SET);

    ret = read(fd, read_buf, sizeof(read_buf));
    if (ret < 0) {
        printf("Error read\n");
        return -1;
    }
    printf("read succeeded, read_buf = %s, ret:%d\n", read_buf, ret);

    close(fd);
    return 0;
}