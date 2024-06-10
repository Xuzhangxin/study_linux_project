#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <poll.h>
#include <signal.h>
 
int main() 
{
 
    // 创建socket
    int lFd = socket(PF_INET, SOCK_STREAM, 0);
    if (lFd < 0) 
    {
        printf("socket error\n");
        return -1;
    }

    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
 
    int iRet = 0;
    // 绑定
    iRet = bind(lFd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (iRet < 0) 
    {
        printf("bind error\n");
        return -1;
    }
 
    // 监听
    iRet = listen(lFd, 8);
    if (iRet < 0) 
    {
        printf("listen error\n");
        return -1;
    }
 

    int nFds = 0;
    struct pollfd fds[512] = {0};
    int maxFds = sizeof(fds)/ sizeof(fds[0]);
    for (int i = 0; i < maxFds; i++)
    {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }

    fds[0].fd = lFd;
    fds[0].events = POLLIN;

    while(1) 
    {
        iRet = poll(fds, nFds + 1, -1);
        if (iRet == -1)
        {
            perror("poll error:");
            continue;
        }
        else if (iRet == 0)
        {
            printf("poll return no results\n");
            continue;
        }
        else
        {
            /// new client
            if (fds[0].revents & POLLIN)
            {
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lFd, (struct sockaddr *)&cliaddr, &len);

                printf("new client connect\n");
                for (int i = 1; i < maxFds; i++)
                {
                    if (fds[i].fd == -1)
                    {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        nFds = i > nFds ? i : nFds;
                        printf("new client connect success, fd:%d, nFds:%d\n", fds[i].fd, nFds);
                        break;
                    }
                }
            }

            /// client have data
            for (int i = 1; i <= nFds; i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    char acBuf[1024] = {0};
                    int iLen = read(fds[i].fd, acBuf, sizeof(acBuf));
                    if (iLen == -1)
                    {
                        printf("fd:%d error read ret:%d\n", i, iLen);
                        continue;
                    }
                    else if (iLen == 0)
                    {
                        if (i == nFds)
                        {
                            nFds--;
                        }
                        printf("fd %d closed, relase source, nFds:%d\n", fds[i].fd, nFds);
                        fds[i].fd = -1;
                        fds[i].events = POLLIN;
                        
                    }
                    else
                    {
                        printf("fd %d, recv buf :%s, return ok\n", fds[i].fd, acBuf);
                        write(fds[i].fd, "ok", strlen("ok"));
                    }
                }
            }
        }
    }

    //todo: close fd
    return 0;
}