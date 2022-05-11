#include <sys/types.h>		/* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>	/* for winsize */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif
#include <errno.h>
#include <stdio.h>		/* for convenience */
#include <stdlib.h>		/* for convenience */
#include <stddef.h>		/* for offsetof */
#include <string.h>		/* for convenience */
#include <unistd.h>		/* for convenience */
#include <signal.h>		/* for SIG_ERR */
#include <sys/socket.h>
#include <netdb.h>
#include "socket_base.h"

#define BUFSIZE 1024
#define MAX_SLEEP 64

int home_name_get(char **host_name)
{
    char *name = NULL;
    int host_len = 0;
    int ret = 0;

    //获取hostnameMax
    host_len = sysconf(_SC_HOST_NAME_MAX);
    if (host_len < 0) {
        host_len = _SC_HOST_NAME_MAX;
    }
    printf("host_len: %d\n", host_len);

    name = (char *)malloc(host_len);   ///TODO Free
    if (name == NULL) {
        printf("host_name is NULL\n");
        return -1;
    }
    //获取主机name
    ret = gethostname(name, host_len);
    if (ret != 0) {
        printf("gethostname failed:%d\n", ret);
        return ret;       
    }
    printf("host_name: %s\n", name);

    *host_name = name;

    return 0;
}

void host_name_free(char *host_name)
{
    free(host_name);
}

int init_server(int type, const struct sockaddr *addr, socklen_t len, int num)
{
    int fd = 0;
    int ret = 0;
    //1.socket 创建套接字 
    fd = socket(addr->sa_family, type, IPPROTO_IP);
    if (fd < 0) {
        printf("socket error : %d\n", fd);
        return -1;
    }
    //2.bind 绑定套接字与地址
    ret = bind(fd, addr, len);
    if (ret < 0) {
        printf("socket error : %d\n", ret);
        goto err_exit;
    }
    //3.listen 监听连接,前提是有连接类型通信
    if ((type == SOCK_STREAM) || (type == SOCK_SEQPACKET)) {
        ret = listen(fd, num);
        if (ret < 0) {
            printf("listen error : %d\n", ret);
            goto err_exit;
        }
    }

    return fd;

err_exit:
    ret = errno;
    close(fd);  //直到对fd引用的最后一个关闭，才释放网络端点。单一可用shutdown
    errno = ret;    //**close也会设置errno，防止错误码被close错误时改变，所以临时存储再赋值
    return -1;
}

//开启server端，创建ruptime进程并通过管道给到当前进程，然后发送
int start_server(int sockfd)
{
    int fd = 0;
    FILE *fp = NULL;
    char buf[BUFSIZE] = {0};

    //4.获取连接请求，并建立连接，阻塞时,一直等待连接，如果sockfd为非阻塞模式，则不会阻塞等待
    fd = accept(sockfd, NULL, NULL); //不关心客户端标识,获取套接字描述符
    if (fd == -1) {
        printf("accept error: %d\n", fd);
        return -1;
    }
    fp = popen("/usr/bin/uptime", "r");    //快速开启一个匿名管道
    if (fp == NULL) {
        printf("popen error\n");
        return -1;        
    }
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        send(fd, buf, strlen(buf), 0);//(int __fd, const void *__buf, size_t __n, int __flags)
    }

    pclose(fp);
    close(fd);

    return 0;
}


int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t len)
{
    int ret = 0;
    int numsec = 0;
    int sockfd = 0;

    for (numsec = 1; numsec <= MAX_SLEEP; numsec <<= 1) { //num = num << 1         1\2\4\8\16\32\64\128     指数型增长      
        sockfd = socket(domain, type, protocol);
        if (sockfd < 0) {
            printf("socket error : %d\n", sockfd);
            return -1;
        }

        ret = connect(sockfd, addr, len);
        if (ret == 0) {
            //connect success
            return 0;
        }
        
        close(sockfd);//connect 失败时套接字状态是未知的，需要重新关闭和打开

        sleep(numsec);
    }

    return -1;
}

void print_uptime(int scokfd)
{
    int n = 0;
    char buf[BUFSIZE] = {0};

    while(n = recv(scokfd, buf, sizeof(buf), 0) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

    if (n < 0) {
        printf("print_uptime error : %d\n", n);
        return;
    }
    return;
    
}