/**
 * 聊天室服务器程序epoll版本
 *
 */
#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include "tlpi_hdr.h"

#define MAX_EVENT_NUMBER 1024

static int pipefd[2];
static bool stop_server = false;

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
void addfd(int epollfd, int fd)
{
    epoll_event e;
    e.data.fd = fd;
    e.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &e);
    setnonblocking(fd);
}

void sig_handler(int sig)
{
    printf("000:%d\n", sig);

    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}
void addsig(int sig)
{
    struct sigaction sa;

    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sig_handler;

    ssize_t ret = sigaction(sig, &sa, NULL);

    assert(ret != -1);
}
void doit(epoll_event *events, int number, int epollfd, int listenfd, int &user_counter)
{
    for (int i = 0; i < number; i++)
    {
        auto target = events + i;
        auto target_fd = target->data.fd;
        auto target_event = target->events;

        if (target_fd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_address_len = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_address_len);
            assert(connfd >= 0);

            addfd(epollfd, connfd);
        }
        else if ((target_fd == pipefd[0]) && (target_event & EPOLLIN))
        {
            int sig;
            char signals[1024];

            int ret = recv(target_fd, signals, sizeof(signals), 0);

            if (ret == -1)
            {
            }
            else if (ret == 0)
            {
            }
            else
            {
                for (size_t i = 0; i < ret; i++)
                {
                    auto c = signals[i];
                    printf("111:%d\n", c);
                    switch (signals[i])
                    {
                    case SIGCHLD:
                    case SIGHUP:

                    {
                        continue;
                    }
                    case SIGTERM:
                    case SIGINT:

                    {
                        stop_server = true;
                    }

                    default:
                        break;
                    }
                }
            }
        }
        else if (target_event & EPOLLIN)
        {
            char buf[MAX_EVENT_NUMBER];
            memset(buf, '\0', MAX_EVENT_NUMBER);

            while (1)
            {
                int nread = recv(target_fd, buf, MAX_EVENT_NUMBER - 1, 0);

                if (nread < 0)
                {
                    if (errno == EAGAIN)
                    {
                        printf("nothind to read\n");
                        break;
                    }
                    else
                    {
                        close(target_fd);
                        break;
                    }
                }
                else if (nread == 0)
                {
                    close(target_fd);
                    break;
                }
            }
            printf("read: %s\n", buf);
        }

        else
        {
            /* code */
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

    int user_counter = 0;

    int epollfd = epoll_create(5);
    assert(ret != 1);

    addfd(epollfd, listenfd);
    epoll_event events[MAX_EVENT_NUMBER];

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd);

    assert(ret != -1);

    setnonblocking(pipefd[1]);

    addfd(epollfd, pipefd[0]);

    addsig(SIGHUP);
    addsig(SIGCHLD);
    addsig(SIGTERM);
    addsig(SIGINT);

    while (!stop_server)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            errExit("epoll_wait");
        };

        doit(events, number, epollfd, listenfd, user_counter);
    }
    close(listenfd);
    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}
