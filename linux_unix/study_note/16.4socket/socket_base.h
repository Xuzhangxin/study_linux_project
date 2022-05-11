#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__

int home_name_get(char **host_name);

void host_name_free(char *host_name);

int init_server(int type, const struct sockaddr *addr, socklen_t len, int num);

int start_server(int sockfd);

int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t len);

void print_uptime(int scokfd);
#endif