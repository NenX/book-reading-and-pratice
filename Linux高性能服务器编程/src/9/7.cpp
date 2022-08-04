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
            auto target = fds + i;

            if ((target->fd == listenfd) && (target->revents & POLLIN))
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

                auto clientfd = fds + i;

                clientfd->fd = connfd;
                clientfd->events = POLLIN & POLLRDHUP;
                clientfd->revents = 0;

                auto client_data = users + connfd;

                client_data->address = client_address;
                setnonblocking(connfd);

                printf("comes a new user, now have %d users.\n", user_counter);
            }
            else if (target->revents & POLLERR)
            {
                printf("get an error from %d users.\n", target->fd);
            }
            else if (target->revents & POLLRDHUP)
            {
                users[target->fd] = users[fds[user_counter].fd];
                close(target->fd);
                fds[i] = fds[user_counter];
                i--;
                user_counter--;
                printf("a client left.\n");
            }
            else if (target->revents & POLLIN)
            {
                int connfd = target->fd;
                memset(users[connfd].buf, '\0', BUFFER_SIZE);

                ret = recv(connfd, users[connfd].buf, BUFFER_SIZE - 1, 0);
                printf("get %d bytes of client data %s from %d.\n", ret, users[connfd].buf, connfd);

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
                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;

                        users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                }
            }
            else if (target->revents & POLLOUT)
            {
                int connfd = target->fd;
                if (!users[connfd].write_buf)
                {
                    continue;
                }
                ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);

                users[connfd].write_buf = NULL;
                target->events |= ~POLLOUT;
                target->events |= POLLIN;
            }
        }
    }
    delete[] users;
    close(listenfd);
    return 0;
}
