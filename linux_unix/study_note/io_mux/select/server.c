#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
 
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
 
    int maxFd = lFd;
    fd_set allFdSets, tmpFdSets;
    FD_ZERO(&allFdSets);
    FD_SET(lFd, &allFdSets);
 
    while(1) 
    {
        memcpy(&tmpFdSets, &allFdSets, sizeof(tmpFdSets));


        iRet = select(maxFd + 1, &tmpFdSets, NULL, NULL, 0);
        if (iRet == -1)
        {
            perror("select error:");
            continue;
        }
        else if (iRet == 0)
        {
            printf("select return no results\n");
            continue;
        }
        else
        {
            for (int i = lFd; i < maxFd + 1; i++)
            {
                if (i == lFd)
                {
                    /// new client
                    if (FD_ISSET(lFd, &tmpFdSets))
                    {
                        struct sockaddr_in addr = {0};
                        int iLen = sizeof(addr);
                        int clientFd = accept(lFd, (struct sockaddr *)&addr, &iLen);
                        FD_SET(clientFd, &allFdSets);

                        maxFd = clientFd > maxFd ? clientFd : maxFd;
                        FD_CLR(lFd, &tmpFdSets);
                    }
                }
                else
                {
                    /// msg
                    if (FD_ISSET(i, &tmpFdSets))
                    {
                        char acBuf[1024] = {0};
                        int iLen = read(i, acBuf, sizeof(acBuf));
                        if (iLen == -1)
                        {
                            printf("fd:%d error read ret:%d\n", i, iLen);
                            continue;
                        }
                        else if (iLen == 0)
                        {
                            printf("fd %d closed\n", i);
                        }
                        else
                        {
                            printf("fd %d, recv buf :%s, return ok\n", i, acBuf);
                            write(i, "ok", strlen("ok"));
                        }
                    }
                }

                FD_CLR(i, &tmpFdSets);
            }
        }

    }
 
    return 0;
}