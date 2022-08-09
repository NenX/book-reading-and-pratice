// sigurg.cpp
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define LISTEN_NUM 5
#define BUFFER_SIZE 1024

static int connfd; //客户端的fd

void add_signal(int signal_no, void (*sig_handler)(int signal_no));

void sig_handler(int signal_no);

int main(int argc, char *argv[])
{
    const char *ip;
    int port, server_fd;
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

    //创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //初始化服务器地址
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_address.sin_addr) == -1)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    //绑定服务端地址
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    //开启监听
    if (listen(server_fd, LISTEN_NUM) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //接受客户端连接
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    bzero(&client_address, sizeof(client_address));

    if ((connfd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_len)) == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        //添加SIGURG信号处理函数
        add_signal(SIGURG, sig_handler);

        //使用SIGURG之前，必须设置socket的宿主进程或进程组
        fcntl(connfd, F_SETOWN, getpid());

        int recv_ret_value;
        char recv_buffer[BUFFER_SIZE];
        //接收普通数据
        while (1)
        {
            bzero(recv_buffer, sizeof(recv_buffer));
            //接收数据
            recv_ret_value = recv(connfd, recv_buffer, sizeof(recv_buffer - 1), 0);
            if (recv_ret_value < 0)
            {
                perror("recv");
                close(connfd);
                close(server_fd);
                exit(EXIT_FAILURE);
            }
            else if (recv_ret_value == 0)
            {
                break;
            }
            else
            {
                printf("recv normal data:%s ,%d bytes\n", recv_buffer, recv_ret_value);
            }
        }
        close(connfd);
    }

    close(server_fd);
    exit(EXIT_SUCCESS);
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
