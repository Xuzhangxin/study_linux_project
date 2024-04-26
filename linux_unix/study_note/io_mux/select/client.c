#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
 
int main() {
 
    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }
 
    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);
 
    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));
 
    if(ret == -1){
        perror("connect");
        return -1;
    }
 
    int num = 0;
    while(1) {
        char sendBuf[1024] = {0};
        sprintf(sendBuf, "data%d", num++);
        printf("write buf:%s\n", sendBuf);
        write(fd, sendBuf, strlen(sendBuf) + 1);
 
        char recvBuf[1024] = {0};
        // 接收
        int len = read(fd, recvBuf, sizeof(recvBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", recvBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
        sleep(1);
    }
 
    close(fd);
 
    return 0;
}