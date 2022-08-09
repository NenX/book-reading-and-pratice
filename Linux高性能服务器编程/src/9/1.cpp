/**
 * 同时接收普通数据和带外数据
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
#include <tlpi_hdr.h>
#define USER_LIMIT 5

#define BUFFER_SIZE 65535

static char buffer[BUFFER_SIZE];
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

    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_address_len);

    if (connfd < 0)
    {
        close(listenfd);
        errExit("accept");
    }

    fd_set read_fds, exception_fds;

    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    while (1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        FD_SET(connfd, &read_fds);
        FD_SET(connfd, &exception_fds);
        ret = select(connfd + 1, &read_fds, NULL, &exception_fds, NULL);
        if (ret < 0)
        {
            printf("select failed.\n");
            break;
        }
        if (FD_ISSET(connfd, &read_fds))
        {
            int nread = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
            if (nread <= 0)
            {
                break;
            }
            printf("get %d bytes of normal data: %s \n", nread, buffer);
        }
        else if (FD_ISSET(connfd, &exception_fds))
        {
            printf("get oob\n");

            int nread = recv(connfd, buffer, BUFFER_SIZE - 1, MSG_OOB);
            if (nread <= 0)
            {
                break;
            }
            printf("get %d bytes of oob data: %s \n", nread, buffer);
        }
    }
    close(listenfd);
    return 0;
}
