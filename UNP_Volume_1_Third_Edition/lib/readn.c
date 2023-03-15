#include "readn.h"

ssize_t readn(int fd, void *vptr, size_t n)
{
    ssize_t nread = 0;
    size_t nleft = n;
    void *curPos = vptr;
    while (nleft > 0)
    {
        if ((nread = read(fd, curPos, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
        {
            break;
        }
        nleft = nleft - nread;
        curPos += nread;
    }
    return n - nleft;
}