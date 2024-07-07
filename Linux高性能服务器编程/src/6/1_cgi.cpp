#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

int main(int argc, char const *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int port = 1122, client_port, server_fd, client_fd, ret, client_addr_len, reuseport = 1, reuseaddr = 1;
    // const char *host = "172.20.243.121";
    const char *host = "0.0.0.0";
    char client_host[INET_ADDRSTRLEN];
    if (argc > 1)
    {
        port = atoi(argv[0]);
    }

    if (argc > 2)
    {
        host = argv[1];
    }

    std::cout << "port:" << port << "; host:" << host << std::endl;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &server_addr.sin_addr);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuseaddr, sizeof(reuseaddr));

    assert(server_fd > -1);
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < -1)
    {
        err_exit("bind");
    }

    ret = listen(server_fd, 511);
    if (ret < -1)
    {
        err_exit("listen");
    }

    while (1)
    {
        std::cout << "before accept" << std::endl;
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if (client_fd < 0)
        {
            printf("client accept failed, errno: %d", errno);
        }
        else
        {
            sleep(1);

            inet_ntop(AF_INET, &client_addr.sin_addr, client_host, INET_ADDRSTRLEN);
            client_port = ntohs(client_addr.sin_port);
            close(STDOUT_FILENO);
            dup(client_fd);

            printf("client fd: %d, client host: %s, client port: %d \n", client_fd, client_host, client_port);
            std::cout << "client host:" << client_host << "; client port: " << client_port << std::endl;
            close(client_fd);
            close(STDOUT_FILENO);

        }
    }

    return 0;
}
