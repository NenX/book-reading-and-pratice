#include "writen.h"

ssize_t writen(int fd, void *vptr, size_t n)
{
    ssize_t nwritten = 0;
    size_t nleft = n;
    char *pos = vptr;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, pos, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
            {
                nwritten = 0;
            }
            else
            {
                return -1;
            }
        }
        nleft -= nwritten;
        pos += nwritten;
    }
    return n;
}