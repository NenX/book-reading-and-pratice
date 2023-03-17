#include "unp.h"
#include "wrapper.h"
#include <time.h>

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    time_t ticks;
    char buf [MAXLINE];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    while (1)
    {
        len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &len);

        printf("Connection from %s, port %d, server port %d\n",
        Inet_ntop(AF_INET,&cliaddr.sin_addr,buf,MAXLINE),
        ntohs(cliaddr.sin_port),
        ntohs(servaddr.sin_port)
        );

        ticks = time(NULL);
        snprintf(buf,MAXLINE,"ticks: %.24s\r\n",ctime(&ticks));

        Write(connfd,buf,MAXLINE);

        Close(connfd);
    }
}