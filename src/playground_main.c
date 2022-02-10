#define _XOPEN_SOURCE

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#define MAX_NUM 9999
char buf[MAX_NUM];
int main(int argc, char *argv[])
{
    char a[5];
    struct ss
    {
        int i;
        char d[];

    };
    

    printf("????:%u,%u\n", sizeof(a), sizeof(struct ss));
}