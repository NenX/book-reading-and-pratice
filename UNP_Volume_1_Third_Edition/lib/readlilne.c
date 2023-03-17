#include "readlilne.h"

typedef ssize_t (*_read)(int __fd, void *__buf, size_t __nbytes);

ssize_t _readline(int fd, void *vptr, size_t maxlen, _read yourRead, size_t handleEINTR)
{
    int n;
    int nread = 0;
    char *pos = vptr;
    char c;

    for (n = 1; n < maxlen; n++)
    {
    again:
        if ((nread = yourRead(fd, &c, 1)) == 1)
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
            if (errno == EINTR && handleEINTR == 1)
            {
                goto again;
            }
            return -1;
        }
    }
    *pos = 0;
    return n;
}

static size_t myread(int fd, char *vptr)
{
    while (read_cnt <= 0)
    {
    again:
        if ((read_cnt = read(fd, read_buf, MAXLINE)) < 0)
        {
            if (errno == EINTR)
            {
                goto again;
            }
            return -1;
        }
        else if (read_cnt == 0)
        {
            return 0;
        }
        read_ptr = read_buf;
    }

    *vptr = *read_ptr++;
    read_cnt--;
    return 1;
}

ssize_t readline_slow(int fd, void *vptr, size_t maxlen)
{
    return _readline(fd, vptr, maxlen, read, 1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    return _readline(fd, vptr, maxlen, myread, 0);
}

ssize_t readlinebuf(void **vptrptr)
{
    if (read_cnt)
    {
        *vptrptr = read_ptr;
    }
    return read_cnt;
}
