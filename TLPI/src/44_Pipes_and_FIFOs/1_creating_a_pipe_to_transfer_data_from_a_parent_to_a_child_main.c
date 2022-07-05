#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int fields[2];
    char buf[FILENAME_MAX];
    if (argc != 2)
        usageErr("%s string\n", argv[0]);
    if (pipe(fields) == -1)
        errExit("pipe");
    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        if (close(fields[1]) == -1)
            errExit("close");
        while (read(fields[0], buf, FILENAME_MAX) > 0)
        {
            // printf("child read: %s", buf); _exit will not flush !!
            write(STDOUT_FILENO, buf, strlen(buf));
        }
        if (close(fields[0]) == -1)
            errExit("close");
        _exit(EXIT_SUCCESS);
    default:
        if (close(fields[0]) == -1)
            errExit("close p1");

        if (write(fields[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
            fatal("parent partial/failed write\n");
        if (close(fields[1]) == -1)
            errExit("close p2");
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
    return 0;
}
