/**
 * 聊天室服务器程序epoll版本
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

#define USER_LIMIT 5

#define BUFFER_SIZE 64

#define FD_LIMIT 65535

struct client_data
{
    sockaddr_in address;
    char *write_buf;
    char buf[BUFFER_SIZE];
};

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
void rmfd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

void modfd(int epollfd, int fd, bool is_in)
{
    epoll_event e;
    e.data.fd = fd;
    e.events = is_in ? EPOLLIN | EPOLLET : EPOLLOUT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &e);
}
void doit(epoll_event *events, int number, int epollfd, int listenfd, client_data *users, unsigned int *user_fds, int &user_counter)
{
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < number; i++)
    {
        auto target = events + i;
        auto target_fd = target->data.fd;
        auto target_event = target->events;
        auto target_user = users + target_fd;

        if (target_fd == listenfd)
        {
            sockaddr_in client_address;
            socklen_t client_address_len = sizeof(client_address);

            int connfd = accept(listenfd, (sockaddr *)&client_address, &client_address_len);
            if (connfd < 0)
            {
                printf("errno is %d\n", errno);
                continue;
            }
            if (user_counter >= USER_LIMIT)
            {
                auto info = "too many users.\n";
                printf("%s\n", info);
                send(connfd, info, strlen(info), 0);
                close(connfd);
                continue;
            }
            auto user = users + connfd;
            user->address = client_address;

            addfd(epollfd, connfd);
            *(user_fds + user_counter) = connfd;
            user_counter++;
            printf("comes a new user, now have %d users.\n", user_counter);
        }
        else if (target_event & EPOLLIN)
        {
            memset(target_user->buf, '\0', BUFFER_SIZE);

            while (1)
            {
                memset(buffer, '\0', BUFFER_SIZE);

                int ret = recv(target_fd, buffer, BUFFER_SIZE - 1, 0);
                printf("recv return:'%s' %d\n", buffer, ret);
                if (ret < 0)
                {
                    if (errno == EAGAIN)
                    {
                        printf("read later\n");
                        break;
                    }
                    else
                    {
                        user_counter--;
                        rmfd(epollfd, target_fd);
                        close(target_fd);
                        printf("%d exit, now we have %d user\n", target_fd, user_counter);
                        break;
                    }
                }
                else if (ret == 0)
                {
                    close(target_fd);
                }
                else
                {
                    strcat(target_user->buf, buffer);

                    for (int i = 0; i < user_counter + 1; i++)
                    {
                        auto fd = user_fds[i];
                        if (target_fd == fd)
                        {
                            continue;
                        }
                        auto user = users + fd;
                        user->write_buf = target_user->buf;

                        modfd(epollfd, fd, false);
                    }
                }
            }
        }
        else if (target_event & EPOLLOUT)
        {
            if (!target_user->write_buf)
            {
                continue;
            }
            printf("send %d: %s\n", target_fd, target_user->write_buf);
            int ret = send(target_fd, target_user->write_buf, strlen(target_user->write_buf), 0);

            target_user->write_buf = NULL;
            modfd(epollfd, target_fd, true);
        }
        else if (target_event & EPOLLERR)
        {
            printf("EPOLLERR! \n");
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

    int user_counter = 0;

    int epollfd = epoll_create(5);
    assert(ret != 1);

    addfd(epollfd, listenfd);
    epoll_event events[USER_LIMIT + 1];

    client_data users[FD_LIMIT];
    unsigned int user_fds[USER_LIMIT];
    while (1)
    {
        int number = epoll_wait(epollfd, events, user_counter + 1, -1);
        assert(number >= 0);

        doit(events, number, epollfd, listenfd, users, user_fds, user_counter);
    }
    close(listenfd);
    return 0;
}
