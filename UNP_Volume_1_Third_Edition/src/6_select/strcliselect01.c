#include "unp.h"
#include "wrapper.h"
#include "readlilne.h"
#include "readn.h"
#include "writen.h"
#define SOCK_NUM 5
void str_cli(FILE *int_stream, int sockfd)
{
    fd_set rset;
    int fd_no = fileno(int_stream);
    int max_fd = max(fd_no, sockfd);
    char buf[MAXLINE];
    int read_n, write_n;
    while (1)
    {
        FD_SET(sockfd, &rset);
        FD_SET(fd_no, &rset);
        Select(max_fd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset))
        {
            fputs("sockfd can read\n", stdout);

            bzero(buf, MAXLINE);
            if ((read_n = readline(sockfd, buf, MAXLINE)) < 0)
            {
                err_sys("str_cli: read -1");
            }
            if (read_n == 0)
            {
                err_sys("str_cli: read 0 EOL");
            }
            fputs(buf, stdout);
        }
        if (FD_ISSET(fd_no, &rset))
        {
            fputs("input can read\n", stdout);

            bzero(buf, MAXLINE);
            if (fgets(buf, MAXLINE, int_stream) == NULL)
            {
                return;
            }
            writen(sockfd, buf, strlen(buf));
        }
    }
}

void main(int argc, char **argv)
{
    struct sockaddr_in servaddr;
    int sockfd[SOCK_NUM];

    if (argc < 2)
    {
        err_quit("Usage: a.out <IPaddress>");
    }

    for (size_t i = 0; i < SOCK_NUM; i++)
    {
        sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        Inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);
        Connect(sockfd[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
    }
    str_cli(stdin, sockfd[0]);
}