/**
 * 聊天室客户端程序
 *
 */
#define _GNU_SOURCE 1
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
#include <poll.h>
#define MAX_INPUT_NUMBER 32768
#define BUFFER_SIZE 64

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

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr)) < 0)
    {
        printf("connection failed\n");
        close(sockfd);
        return 1;
    }

    pollfd fds[2];

    auto fd0 = fds;
    auto fd1 = fds + 1;

    fd0->fd = 0;
    fd0->events = POLLIN;
    fd0->revents = 0;

    fd1->fd = sockfd;
    fd1->events = POLLIN & POLLRDHUP;
    fd1->revents = 0;

    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    int ret = pipe(pipefd);
    assert(ret != -1);

    while (1)
    {
        ret = poll(fds, 2, -1);
        printf("poll return: %d_%d %d_%d \n",fds[0].revents, fd0->revents, fds[1].revents, fd1->revents);
        if (ret < 0)
        {
            printf("poll failure\n");
            break;
        }

        if (fd1->revents & POLLRDHUP)
        {
            printf("server close the connection\n");
            break;
        }
        else if (fd1->revents & POLLIN)
        {
            memset(read_buf, '\0', BUFFER_SIZE);
            recv(fd1->fd, read_buf, BUFFER_SIZE - 1, 0);
            printf("read from server: %s", read_buf);
        }

        if (fd0->revents & POLLIN)
        {
            splice(fd0->fd, NULL, pipefd[1], NULL, MAX_INPUT_NUMBER, SPLICE_F_MORE & SPLICE_F_MOVE);
            splice(pipefd[0], NULL, fd1->fd, NULL, MAX_INPUT_NUMBER, SPLICE_F_MORE & SPLICE_F_MOVE);
        }
    }
    close(sockfd);
    return 0;
}
