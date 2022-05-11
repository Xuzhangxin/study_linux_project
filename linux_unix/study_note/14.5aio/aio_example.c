// ///异步IO例程，使用信号进行异步通知
// #include <stdio.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <string.h>
// #include <aio.h>
// #include <fcntl.h>
// #include <unistd.h>

// void sigio_handler(int signo, siginfo_t *info, void *context)
// {
//     struct aiocb *req = NULL;
//     int ret = 0;
//     printf("enter sigio_handler\n");
//     switch (signo)
//     {
//         case SIGIO:
//             req = info->si_value.sival_ptr;
//             if (aio_error(req) == 0) {
//                 ret = aio_return(req);
//                 if (ret <= 0) { //-1为自身函数出错 其他返回值为异步操作的结果
//                     fprintf(stderr, "aio_return ret:%d\n", ret);
//                 }
//                 fprintf(stdout, "read:%s\n", req->aio_buf);
//                 printf("aio finish");
//             }    
//             break;
        
//         default:
//             break;
//     }
// }

// void setup_io(int fd) 
// {
//     int rt = 0;
//     struct sigaction sigio_action;
//     struct aiocb sigio_aiocb;

//     memset(&sigio_action, 0, sizeof(sigio_action));
//     memset(&sigio_aiocb, 0, sizeof(sigio_aiocb));

//     ///1.映射sigio处理函数
//     sigemptyset(&sigio_action.sa_mask);
//     sigio_action.sa_sigaction = sigio_handler;
//     sigio_action.sa_flags = SA_SIGINFO;

//     rt = sigaction(SIGIO, &sigio_action, NULL);
//     if (rt != 0) {
//         fprintf(stderr, "sigaction err:%d", rt);
//         return ;
//     }
//     printf("preapare aio_read\n");
//     ///2.发起异步aio_read
//     sigio_aiocb.aio_fildes = fd;
//     sigio_aiocb.aio_buf = malloc(1024+1);
//     sigio_aiocb.aio_nbytes = 1024;
//     sigio_aiocb.aio_offset = 0;
//     sigio_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
//     sigio_aiocb.aio_sigevent.sigev_signo  = SIGIO;
//     sigio_aiocb.aio_sigevent.sigev_value.sival_ptr = &sigio_aiocb;
//     rt = aio_read(&sigio_aiocb);
//     if (rt != 0) {
//         fprintf(stderr, "sigaction err:%d", rt);
//         return ;
//     }
//     printf("aio_read finnish\n");
// }

// int main(int argc, char **argv)
// {
//     int fd = -1, ret = -1;

//     fd = open("./file.txt", O_RDONLY);
//     if (fd < 0) {
//         perror("open file.txt");
//         return -1;
//     }
//     printf("open file.txt successfully opened\n");
//     setup_io(fd);

//     while(1) {
//         sleep(1);
//     }
// }




// ///异步IO例程，使用回调进行异步通知
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>

void handler(__sigval_t val)
{
    struct aiocb *req = NULL;
    int ret = 0;
    printf("enter handler\n");
    req = val.sival_ptr;
    if (aio_error(req) == 0) {
        ret = aio_return(req);
        if (ret <= 0) { //-1为自身函数出错 其他返回值为异步操作的结果
            fprintf(stderr, "aio_return ret:%d\n", ret);
        }
        fprintf(stdout, "read:%s\n", req->aio_buf);
        printf("aio finish\n");
    }
}

void setup_io(int fd) 
{
    int rt = 0;
    struct aiocb sigio_aiocb;

    memset(&sigio_aiocb, 0, sizeof(sigio_aiocb));

    printf("preapare aio_read\n");
    ///2.发起异步aio_read
    sigio_aiocb.aio_fildes = fd;
    sigio_aiocb.aio_buf = malloc(1024+1);
    sigio_aiocb.aio_nbytes = 1024;
    sigio_aiocb.aio_offset = 0;///异步io另起一个线程
    sigio_aiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;
    sigio_aiocb.aio_sigevent.sigev_value.sival_ptr = &sigio_aiocb;
    sigio_aiocb.aio_sigevent._sigev_un._sigev_thread._attribute = NULL;
    sigio_aiocb.aio_sigevent._sigev_un._sigev_thread._function = handler;
    rt = aio_read(&sigio_aiocb);
    if (rt != 0) {
        fprintf(stderr, "sigaction err:%d", rt);
        return ;
    }
    printf("aio_read finnish\n");
}

int main(int argc, char **argv)
{
    int fd = -1, ret = -1;

    fd = open("./file.txt", O_RDONLY);
    if (fd < 0) {
        perror("open file.txt");
        return -1;
    }
    printf("open file.txt successfully opened\n");
    setup_io(fd);

    while(1) {
        sleep(1);
    }
}

///实现一个异步框架