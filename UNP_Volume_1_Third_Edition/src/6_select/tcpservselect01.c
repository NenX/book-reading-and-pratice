#include "unp.h"
#include "wrapper.h"
#include "readlilne.h"
#include "readn.h"
#include "writen.h"
#define SOCK_NUM 5
void str_cli(FILE *int_stream, int sockfd)
{
}

void main(int argc, char **argv)
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    ssize_t n, listenfd, nready, connfd, sockfd, maxfd, maxi, client[FD_SETSIZE];
    fd_set rset, allset;
    size_t i;
    char buf[MAXLINE];
    char buf2[MAXLINE];

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    FD_ZERO(&rset);
    FD_ZERO(&allset);

    FD_SET(listenfd, &allset);

    for (i = 0; i < FD_SETSIZE; i++)
    {
        client[i] = -1;
    }
    maxfd = listenfd;
    maxi = -1;

    while (1)
    {
        rset = allset;
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);
        printf("zzzxxx:%d\n", nready);
        if (FD_ISSET(listenfd, &rset))
        {
            printf("Accept\n");

            bzero(&cliaddr, sizeof(cliaddr));
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            FD_SET(connfd, &allset);
            for (i = 0; i < FD_SETSIZE; i++)
            {
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }
            if (i >= FD_SETSIZE)
                err_sys("too many clients\n");

            if (i > maxi)
                maxi = i;
            if (connfd > maxfd)
                maxfd = connfd;
            if (--nready <= 0)
                continue;
        }
        for (i = 0; i < FD_SETSIZE; i++)
        {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset))
            {
                bzero(buf,MAXLINE);
                printf("start read client:%d\n");

                n = readline_slow(sockfd, buf, MAXLINE);
                printf("read client:%d\n", n);

                if (n == 0)
                {
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    Close(sockfd);
                }
                else
                {
                    snprintf(buf2, MAXLINE, "[??]:%s", buf);
                    writen(sockfd, buf2, strlen(buf2));
                }
                if (--nready <= 0)
                    continue;
            }
        }
    }
}