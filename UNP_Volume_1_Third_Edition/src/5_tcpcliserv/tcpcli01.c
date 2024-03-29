#include "unp.h"
#include "wrapper.h"
#include "writen.h"
#include "readn.h"
#include "readlilne.h"
void str_cli(FILE *fp, int sockfd)
{
    int n;
    char sendline[MAXLINE], recv[MAXLINE];

    while (fgets(sendline, MAXLINE, fp) != NULL)
    {

        writen(sockfd, sendline, strlen(sendline));

        n = readline(sockfd, recv, MAXLINE);
        if (n == -1)
            err_sys("str_cli: -1");
        fputs(recv, stdout);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    if (argc != 2)
        err_quit("usage: a.out <IPaddress>");
    servaddr.sin_port = ntohs(SERV_PORT);
    servaddr.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    str_cli(stdin, sockfd);
}