#include "unp.h"
#include "wrapper.h"
#include <time.h>
#include "readn.h"
#include "writen.h"

void sig_chld(int signo)
{
    int pid, status;
    pid = wait(&status);
    printf("child %d terminated\n", pid);
}

void str_echo(int connfd)
{
    char buf[MAXLINE];
    char buf2[MAXLINE];
    int n;

    while (1)
    {
    again:
        n = read(connfd, buf, MAXLINE);
        if (n < 0)
        {
            if (errno == EINTR)
                goto again;
            err_sys("str_echo");
        }
        else if (n == 0)
        {
            break;
        }

        snprintf(buf2, MAXLINE, "aaa:%s", buf);

        writen(connfd, buf2, strlen(buf2));
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    int childpid;

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);
    while (1)
    {
        len = sizeof(cliaddr);

        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &len);
        childpid = Fork();
        printf("pid: %d\n", childpid);

        if (childpid == 0)
        {
            Close(listenfd);
            // do
            str_echo(connfd);
            exit(0);
        }
        Close(connfd);
    }
}