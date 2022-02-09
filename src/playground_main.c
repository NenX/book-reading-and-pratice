#define _XOPEN_SOURCE

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#define MAX_NUM 9999
char buf[MAX_NUM];
int main(int argc, char *argv[])
{
    struct stat statbuf;
    stat("/home/noah/a.txt", &statbuf);

    struct tm myTm;
    char *result = strptime("2000-01-01 11:11:11", "%Y-%m-%d %H:%M:%S", &myTm);

    struct utimbuf _utimbuf;
    _utimbuf.actime = statbuf.st_atime;
    _utimbuf.modtime = mktime(&myTm);

    int status = utime("/home/noah/a.txt", &_utimbuf);

    printf("????:%d\n", status);
}