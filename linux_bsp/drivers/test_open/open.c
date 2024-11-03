#include <stdio.h>
int main(int argc, char **argv)
{
    int fd = open("./test.txt", 0777);
    if (fd < 0) {
        printf("test open failed");
        return -1;
    }

    write(fd, "123", 3);

    close(fd);

    return 0;
}