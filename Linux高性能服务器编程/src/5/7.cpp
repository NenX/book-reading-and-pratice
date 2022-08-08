/**
 * 接收带外数据
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include "tlpi_hdr.h"

#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
int main(int argc, char const *argv[])
{
    const char *ip;
    int port;
    if (argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        ip = "127.0.0.1";
        port = 1234;
    }
    else
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(address));

    address.sin_family = AF_INET;

    inet_pton(AF_INET, ip, &address.sin_addr);

    address.sin_port = htons(port);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(struct sockaddr));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    struct sockaddr_in client_address;
    socklen_t client_addresslength = sizeof(client_address);
    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addresslength);

    memset(buffer, '\0', BUFFER_SIZE);
    ret = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
    printf("got %d bytes of normal data: %s\n",ret,buffer);

    
    memset(buffer, '\0', BUFFER_SIZE);
    ret = recv(connfd, buffer, BUFFER_SIZE - 1, MSG_OOB);
    printf("got %d bytes of oob data: %s\n",ret,buffer);


    memset(buffer, '\0', BUFFER_SIZE);
    ret = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
    printf("got %d bytes of normal data: %s\n",ret,buffer);




    close(listenfd);

    return 0;
}
