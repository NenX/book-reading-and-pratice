/**
 * 用 SIGURG 检测带外数据是否到达
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

#define BUFFER_SIZE 1024

typedef void (*my_sig_handler_t)(int);
static bool stop_server = false;
static int connfd;
static char buffer[BUFFER_SIZE];
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
void sig_handler(int signal_no)
{
    //在这个信号处理函数中，我们接收带外数据
    int save_errno = errno;

    int recv_ret_value;
    char recv_buffer[BUFFER_SIZE];
    bzero(recv_buffer, sizeof(recv_buffer));
    //接收带外数据
    recv_ret_value = recv(connfd, recv_buffer, sizeof(recv_buffer - 1), 0);
    printf("recv oob data:%s ,%d bytes\n", recv_buffer, recv_ret_value);

    errno = save_errno;
}

void sig_urg_handler(int sig)
{
    printf("aaaaxxxxxxxxx\n");
    int save_errno = errno;

    memset(buffer, '\0', BUFFER_SIZE);
    int nread = recv(connfd, buffer, BUFFER_SIZE - 1, MSG_OOB);
    printf("get %d bytes of oob data: %s \n", nread, buffer);

    errno = save_errno;
}
void addsig(int sig, my_sig_handler_t handler)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sig_urg_handler;

    ssize_t ret = sigaction(sig, &sa, NULL);

    assert(ret != -1);
}
void add_signal(int signal_no, void (*sig_handler)(int signal_no))
{
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    //对信号进行处理
    sigaction(signal_no, &act, NULL);
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
    connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_address_len);
    assert(connfd >= 0);

    add_signal(SIGURG, sig_urg_handler);
    fcntl(connfd, F_SETOWN, getpid());
    char buffer [BUFFER_SIZE];
    while (1)
    {
        memset(buffer, '\0', BUFFER_SIZE);
        int nread = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
        if (nread <= 0)
        {
            break;
        }
        printf("get %d bytes of normal data: %s \n", nread, buffer);
    }
    close(listenfd);

    return 0;
}
