/**
 * 发送带外数据
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

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

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

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    int ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (ret == -1)
    {
        errExit("connect");
    }

    char oob_data[] = "01234";
    char oob_data2[] = "56789";
    char normal_data[] = "normal";

    send(sockfd, normal_data, strlen(normal_data), 0);
    send(sockfd, oob_data, strlen(oob_data), MSG_OOB);

    send(sockfd, normal_data, strlen(normal_data), 0);
    send(sockfd, oob_data2, strlen(oob_data2), MSG_OOB);

    close(sockfd);

    return 0;
}
