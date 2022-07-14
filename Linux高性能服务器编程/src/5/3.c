#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"
static Boolean stop = FALSE;
static void term_handler(int sig)
{
    printf("terminal!!\n");

    stop = TRUE;
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in servaddr;
    int listenfd, connfd;
    int ip;
    int port;
    int backlog;
    int ret;

    if (argc <= 3)
    {
        printf("usage: %s ip_address port_number backlog\n", argv[0]);
        return 1;
    }

    signal(SIGTERM, term_handler);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    backlog = atoi(argv[3]);

    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret == -1)
    {
        err_exit("bind");
    };

    ret = listen(listenfd, backlog);
    if (ret == -1)
    {
        err_exit("listen");
    };

    while (!stop)
    {
        sleep(2);
    }


    close(listenfd);
    return 0;
}
