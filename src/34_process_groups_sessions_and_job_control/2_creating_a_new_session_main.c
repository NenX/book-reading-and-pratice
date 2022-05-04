#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
int main(int argc, char *argv[])
{
    printf("parent: PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(),
           (long)getpgrp(), (long)getsid(0));

    if (fork() != 0) /* Exit if parent, or on error */
        _exit(EXIT_SUCCESS);
    printf("child: PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(),
           (long)getpgrp(), (long)getsid(0));
    if (setsid() == -1)
        errExit("setsid");
    printf("child: PID=%ld, PGID=%ld, SID=%ld\n", (long)getpid(),
           (long)getpgrp(), (long)getsid(0));
    if (open("/dev/tty", O_RDWR) == -1)
        errExit("open /dev/tty");
    exit(EXIT_SUCCESS);
}