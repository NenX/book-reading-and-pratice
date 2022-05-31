#include <unistd.h>
#include "tlpi_hdr.h"

int main()
{
    int fields[2];
    char buf[FILENAME_MAX];
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
        while (read(fields[0], buf, FILENAME_MAX) != -1)
        {
            printf("child read: %s",buf);
        }
        break;

    default:
        if (close(fields[0]) == -1)
            errExit("close");

        break;
    }
    return 0;
}