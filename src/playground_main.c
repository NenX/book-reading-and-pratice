#define _XOPEN_SOURCE

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_NUM 9999
char buf[MAX_NUM];

void fn()
{
    int pfd[2];
    if (pipe(pfd) == -1)
        errExit("pipe");

    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        if (close(pfd[0]) == -1)
            errExit("close");
        if (pfd[1] != STDOUT_FILENO)
        {
            if (dup2(pfd[1], STDOUT_FILENO) == -1)
                errExit("dup2");

            if (close(pfd[1]) == -1)
                errExit("close");
        }
        execlp("ls", "ls", (char *)NULL);
        errExit("execlp");

        break;
    default:
        break;
    }

    switch (fork())
    {
    case -1:
        errExit("fork2");
        break;
    case 0:
        if (close(pfd[1]) == -1)
            errExit("close2");
        if (pfd[0] != STDIN_FILENO)
        {
            if (dup2(pfd[0], STDIN_FILENO) == -1)
                errExit("dup2 2");

            if (close(pfd[0]) == -1)
                errExit("close 2");
        }
        execlp("grep", "grep", "44", (char *)NULL);
        errExit("execlp");

        break;
    default:
        break;
    }

    if (close(pfd[0]) == -1)
        errExit("close2");

    if (close(pfd[1]) == -1)
        errExit("close2");

    if (wait(NULL) == -1)
        errExit("wait 1");

    if (wait(NULL) == -1)
        errExit("wait 2");
}
int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "n")) != -1)
    {
        printf("argc:%d, optind: %d\n", argc, optind);
    }
}