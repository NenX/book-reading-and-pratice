#include "unp.h"
#include "wrapper.h"
#include "readlilne.h"
#include "readn.h"
#include "writen.h"
#include <limits.h>
#define OPEN_MAX 1024
#define SOCK_NUM 5
void str_cli(FILE *int_stream, int sockfd)
{
}

void main(int argc, char **argv)
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    size_t i;
    struct pollfd client[OPEN_MAX];
    int listenfd, connfd, sockfd, maxi = 0, nready = 0, n = 0;
    char buf[MAXLINE];
    char buf2[MAXLINE];

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_family = AF_INET;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        err_sys("setsockopt");

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for (i = 0; i < OPEN_MAX; i++)
    {
        client[i].fd = -1;
    }
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    while (1)
    {
        nready = Poll(client, maxi + 1, INFTIM);

        if (client[0].revents & POLLRDNORM)
        {
            clilen = sizeof(cliaddr);
            bzero(&cliaddr, clilen);
            connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            for (i = 0; i < OPEN_MAX; i++)
            {
                if (client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    client[i].events = POLLRDNORM;
                    break;
                }
            }
            if (i >= OPEN_MAX)
                err_sys("too many clients");

            if (maxi < i)
                maxi = i;
            if (--nready <= 0)
                continue;
        }

        for (i = 1; i < OPEN_MAX; i++)
        {
            if ((sockfd = client[i].fd) < 0)
                continue;
            if (client[i].revents & (POLLRDNORM | POLLERR))
            {
                if ((n = readline_slow(sockfd, buf, MAXLINE)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        printf("Reset!!\n");
                        Close(sockfd);
                        client[i].fd = -1;
                    }
                    else
                        err_sys("read error!");
                }
                else if (n == 0)
                {
                    printf("Close!!\n");
                    Close(sockfd);
                    client[i].fd = -1;
                }
                else
                {
                    snprintf(buf2, MAXLINE, "[sb]:%s", buf);
                    writen(sockfd, buf2, strlen(buf2));
                }
                if (--nready <= 0)
                    break;
            }
        }
    }
}