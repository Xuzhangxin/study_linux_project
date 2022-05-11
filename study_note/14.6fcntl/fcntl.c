#include "apue.h"
#include <fcntl.h>


pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_whence = whence;
    lock.l_start = offset;
    lock.l_len = len;

    if (fcntl(fd, F_GETFL, &lock) < 0) {
        perror("f_cntl error");
        return -1;
    }

    if (lock.l_type == F_UNLCK) {
        printf("l_type is %d\n", lock.l_type);
        return 0;
    }
    printf("l_type11 is %d\n", lock.l_pid);
    return lock.l_pid;
}

static void lockbytes(const char *name, int fd, off_t offset, off_t len)
{
    int rt = 0;
    rt = lock_test(fd, F_SETFL, offset, SEEK_SET, len);
    if (rt < 0) {
        err_sys("%s :lock_test error\n", name);
    }
    printf("%s got the lock, byte %lld\n", name, (long long)offset);
}

int main(int argc, char **argv)
{
    //两个进程，各自加锁，然后访问对方的加锁区域，造成sisuoi
    //创建一个文件
    int fd;
    pid_t pid;
    int w_len = 0;

    fd = open("test12.txt", O_CREAT|O_RDWR, 777);
    if (fd < 0) {
        perror("open test.txt");
        exit(-1);
    }
    w_len = write(fd, "abcd", 4);
    if (w_len != 4) {
        perror("write test.txt");
        exit(-1);
    }

    //TELL_WAIT();
    pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        lockbytes("child:", fd, 0, 1);
        // TELL_PARENT(getppid());
        // WAIT_PARENT();
        lockbytes("child:", fd, 1, 1);
    } else if (pid > 0) {
         lockbytes("parent:", fd, 1, 1);
        // // TELL_CHILD(pid);
        // // WAIT_CHILD();
        lockbytes("parent:", fd, 0, 1);
    }

    close(fd);
    exit(0);
}







