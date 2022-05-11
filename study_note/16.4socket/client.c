#include <sys/types.h>		/* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>	/* for winsize */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif

#include <stdio.h>		/* for convenience */
#include <stdlib.h>		/* for convenience */
#include <stddef.h>		/* for offsetof */
#include <string.h>		/* for convenience */
#include <unistd.h>		/* for convenience */
#include <signal.h>		/* for SIG_ERR */
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "socket_base.h"
pthread_attr_t 
int main()
{
    int ret = 0; 
    char *host_name = NULL;
    int scokfd = 0;

    home_name_get(&host_name);

    struct addrinfo hints = {0};///TODO maybe fix
    hints.ai_socktype = SOCK_STREAM;/* SERVICE not supported for `ai_socktype'. EAI_AGAIN */
    hints.ai_flags = AI_CANONNAME;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    struct addrinfo *res_list = NULL;
    struct addrinfo *res = NULL;
    ret = getaddrinfo(host_name, "ruptime", &hints, &res_list);//获取addrinfo
    if (ret != 0) {
        printf("getaddrinfo failed:%d", ret);
        return -1; 
    }
    for (res = res_list; res != NULL; res = res->ai_next) {
        scokfd = connect_retry(res->ai_family, SOCK_STREAM, IPPROTO_IP, res->ai_addr, res->ai_addrlen);//初始化server
        if (scokfd < 0) {
            printf("init_server failed:%d", scokfd);
            return -1;
        }
        print_uptime(scokfd);
    }

    host_name_free(host_name);
}