/**
 * 聊天室服务器程序
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

    client_data *users = new client_data[FD_LIMIT];

    pollfd fds[USER_LIMIT + 1];
    int user_counter = 0;

    for (int i = 1; i < USER_LIMIT; i++)
    {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = listenfd;
    fds[0].events = POLLIN | POLLRDHUP;
    fds[0].revents = 0;
    while (1)
    {
        ret = poll(fds, user_counter + 1, -1);
        if (ret < 0)
        {
            printf("poll failure\n");
            break;
        }
        for (int i = 0; i < user_counter + 1; i++)
        {
            auto target_pollfd = fds + i;
            auto target_user = users + target_pollfd->fd;
            if ((target_pollfd->fd == listenfd) && (target_pollfd->revents & POLLIN))
            {
                struct sockaddr_in client_address;
                socklen_t client_addresslength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addresslength);

                if (connfd < 0)
                {
                    printf("errno is %d", errno);
                    continue;
                }

                if (user_counter >= USER_LIMIT)
                {
                    auto info = "too many users.\n";
                    printf("%s", info);
                    send(connfd, info, strlen(info), 0);
                    close(connfd);
                    continue;
                }
                user_counter++;

                auto clientfd = fds + user_counter;

                clientfd->fd = connfd;
                clientfd->events = POLLIN | POLLRDHUP | POLLERR;
                clientfd->revents = 0;

                auto client_data = users + connfd;

                client_data->address = client_address;
                setnonblocking(connfd);

                printf("comes a new user, now have %d users.\n", user_counter);
            }
            else if (target_pollfd->revents & POLLERR)
            {
                printf("get an error from %d users.\n", target_pollfd->fd);
            }
            else if (target_pollfd->revents & POLLRDHUP)
            {

                auto oofd = fds + user_counter;
                auto oo_user = users + oofd->fd;
                users[target_pollfd->fd] = *oo_user;
                close(target_pollfd->fd);
                fds[i] = *oofd;
                i--;
                user_counter--;
                printf("a client left.\n");
            }
            else if (target_pollfd->revents & POLLIN)
            {
                int connfd = target_pollfd->fd;
                memset(target_user->buf, '\0', BUFFER_SIZE);

                ret = recv(connfd, target_user->buf, BUFFER_SIZE - 1, 0);
                printf("get %d bytes of client data %s from %d.\n", ret, target_user->buf, connfd);

                if (ret < 0)
                {
                    if (errno != EAGAIN)
                    {
                        close(connfd);
                        users[connfd] = users[fds[user_counter].fd];
                        fds[i] = fds[user_counter];
                        i--;
                        user_counter--;
                    }
                }
                else if (ret == 0)
                {
                }
                else
                {
                    for (int j = 1; j <= user_counter; j++)
                    {
                        if (fds[j].fd == connfd)
                        {
                            continue;
                        }
                        fds[j].events &= ~POLLIN;
                        fds[j].events |= POLLOUT;

                        users[fds[j].fd].write_buf = target_user->buf;
                    }
                }
            }
            else if (target_pollfd->revents & POLLOUT)
            {
                int connfd = target_pollfd->fd;
                if (!target_user->write_buf)
                {
                    printf("fd %d has no buf\n", connfd);

                    continue;
                }
                printf("write %s to fd %d\n", target_user->write_buf, connfd);
                ret = send(connfd, target_user->write_buf, strlen(target_user->write_buf), 0);

                target_user->write_buf = NULL;

                target_pollfd->events &= ~POLLOUT;
                target_pollfd->events |= POLLIN;
            }
        }
    }
    delete[] users;
    close(listenfd);
    return 0;
}
