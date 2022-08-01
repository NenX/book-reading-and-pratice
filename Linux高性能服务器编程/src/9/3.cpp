/**
 * epoll LT 和 ET 模式
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
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool enable_et)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et)
    {
        event.events |= EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}
void lt(epoll_event *events, int number, int epollfd, int listenfd)
{
    char buffer[BUFFER_SIZE];
    for (size_t i = 0; i < number; i++)
    {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addresslength = sizeof(client_address);

            int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addresslength);
            addfd(epollfd, connfd, false);
        }
        else if (events[i].events & EPOLLIN)
        {
            printf("event trigger once \n");
            memset(buffer, '\0', BUFFER_SIZE);
            int ret = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (ret <= 0)
            {
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content: %s\n", ret, buffer);
        }
        else
        {
            printf("Something else happened \n");
        }
    }
}

void et(epoll_event *events, int number, int epollfd, int listenfd)
{
    char buffer[BUFFER_SIZE];
    for (size_t i = 0; i < number; i++)
    {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addresslength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addresslength);
            addfd(epollfd, connfd, true);
        }
        else if (events[i].events & EPOLLIN)
        {
            printf("event trigger once \n");

            while (1)
            {
                memset(buffer, '\0', BUFFER_SIZE);

                int ret = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

                if (ret < 0)
                {
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                    {
                        printf("read later\n");
                        break;
                    }
                    close(sockfd);
                    break;
                }
                else if (ret == 0)
                {
                    close(sockfd);
                }
                else
                {
                    printf("get %d bytes of content: %s\n", ret, buffer);
                }
            }
        }
        else
        {
            printf("Something else happened \n");
        }
    }
}
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

    epoll_event events[MAX_EVENT_NUMBER];

    int epollfd = epoll_create(5);
    assert(epollfd != -1);

    addfd(epollfd, listenfd, true);

    while (1)
    {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        printf("epoll_wait return:%d\n", ret);

        if (ret < 0)
        {
            printf("epoll failed\n");
            break;
        }
        // lt(events, ret, epollfd, listenfd);
        et(events, ret, epollfd, listenfd);
    }
    close(listenfd);

    return 0;
}
