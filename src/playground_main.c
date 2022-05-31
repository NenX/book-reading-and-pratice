#define _XOPEN_SOURCE

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <signal.h>
#include <fcntl.h>
#define MAX_NUM 9999
char buf[MAX_NUM];

void fn(int n)
{
    printf("????:%d\n", n);
    sleep(999);
}
int main(int argc, char *argv[])
{
    fprintf(stderr, "Terminal FG process group: %d\n", getpid());
    if (isatty(STDIN_FILENO))
    {
        fprintf(stderr, "heihei\n");
    }
    fprintf(stderr, "in:%d\n", STDIN_FILENO);
    fprintf(stderr, "out:%d\n", STDOUT_FILENO);
}