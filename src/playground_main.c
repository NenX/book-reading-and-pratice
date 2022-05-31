#define _XOPEN_SOURCE

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX_NUM 9999
char buf[MAX_NUM];

void fn(int n)
{
    printf("????:%d\n", n);
    sleep(999);
}
int main(int argc, char *argv[])
{
   printf("abc\n");
   _exit(EXIT_SUCCESS);
}