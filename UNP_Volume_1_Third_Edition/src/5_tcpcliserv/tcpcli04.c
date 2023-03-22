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
    int sockfd[5];
    struct sockaddr_in servaddr;

    if (argc < 2)
    {
        err_quit("usage: tcpcli <IPaddress>");
    }
    for (size_t i = 0; i < 5; i++)
    {
        sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

        Connect(sockfd[i], (SA *)&servaddr, sizeof(servaddr));
    }
    str_cli(stdin, sockfd[0]);
}