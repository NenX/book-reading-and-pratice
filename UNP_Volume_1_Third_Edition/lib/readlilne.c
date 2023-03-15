#include "readlilne.h"

ssize_t readline_slow(int fd, void *vptr, size_t maxlen)
{
    int n;
    int nread = 0;
    char *pos = vptr;
    char c;

    for (n = 1; n < maxlen; n++)
    {
    again:
        if ((nread = read(fd, &c, 1)) == 1)
        {
            *pos++ = c;
            if (c == '\n')
            {
                break;
            }
        }
        else if (nread == 0)
        {
            *pos = 0;
            return n - 1;
        }
        else
        {
            if (errno == EINTR)
            {
                goto again;
            }
            return -1;
        }
    }
    *pos = 0;
    return n;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
 
}

ssize_t readlinebuf(void **vptrptr)
{

}
