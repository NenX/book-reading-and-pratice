#define _GNU_SOURCE 1 // sigaction，看 features.h

#include "wrapper.h"
#include "signal.h"
sigfunc *Signal(int signo, sigfunc *func)
{

    struct sigaction act, oact;

    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = func;
    if (signo == SIGALRM)
    {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; // SunOS 4.x
#endif
    }
    else
    {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }
    if (sigaction(signo, &act, &oact) < 0)
    {
        return SIG_ERR;
    }
    return oact.sa_handler;
}

int Socket(int family, int type, int protocol)
{
    int sockfd;
    if ((sockfd = socket(family, type, protocol)) < 0)
        err_sys("socket error");
    return sockfd;
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (connect(fd, sa, salen) < 0)
        err_sys("connect error");
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
        err_sys("bind error");
}

void Listen(int fd, int backlog)
{
    char *ptr;
    if ((ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);
    if (listen(fd, backlog) < 0)
        err_sys("listen error");
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int sockfd;
    while ((sockfd = accept(fd, sa, salenptr)) < 0)
    {
        if (!(
                errno == EPROTO          // SVR4
                || errno == ECONNABORTED //POSIX
                ))
            err_sys("accept");
    }
    return sockfd;
}

void Close(int fd)
{
    if (close(fd) == -1)
        err_sys("close error");
}

void Getsockname(int sockfd, struct sockaddr *sa, socklen_t *salenptr)
{
    if (getsockname(sockfd, sa, salenptr) < 0)
        err_sys("getsockname error");
}

void Getpeername(int sockfd, struct sockaddr *sa, socklen_t *salenptr)
{
    if (getpeername(sockfd, sa, salenptr) < 0)
        err_sys("getpeername error");
}

void Setsockopt(int sockfd, int level, int optname,
                const void *optval, socklen_t optlen)
{
    if (setsockopt(sockfd, level, optname, optval, optlen) == -1)
        err_sys("setsockopt error");
}

void Getsockopt(int sockfd, int level, int optname,
                void *optval, socklen_t *optlen)
{
    if (getsockopt(sockfd, level, optname, optval, optlen) == -1)
        err_sys("getsockopt error");
}

void Shutdown(int fd, int how)
{
    if (shutdown(fd, how) == -1)
        err_sys("shutdown error");
}

const char *Inet_ntop(int family, const void *src, char *dst, socklen_t size)
{
    const char *ptr;
    if (dst == NULL)
        err_quit("NULL 3th argument to inet_ntop");
    if ((ptr = inet_ntop(family, src, dst, size)) == NULL)
        err_sys("inet_ntop error");
    return ptr;
}

void Inet_pton(int family, const char *src, void *dst)
{
    int n;
    if ((n = inet_pton(family, src, dst)) < 0)
        err_sys("inet_pton error for %s, invalid address family", src);
    else if (n == 0)
        err_quit("inet_pton error for %s, invalid address", src);
}

/**
 *  unix c function wrapper
 */

void *Calloc(size_t n, size_t size)
{
    void *ptr;
    if ((ptr = calloc(n, size)) == NULL)
        err_sys("calloc error");
    return ptr;
}

void *Malloc(size_t size)
{
    void *ptr;
    if ((ptr = malloc(size)) == NULL)
        err_sys("malloc error");
    return ptr;
}

pid_t Fork()
{
    pid_t pid;
    if ((pid = fork()) == -1)
        err_sys("fork error");
    return pid;
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    int n;
    if ((n = read(fd, ptr, nbytes)) == -1)
        err_sys("read error");
    return n;
}

void Write(int fd, void *ptr, size_t nbytes)
{
    if (write(fd, ptr, nbytes) != nbytes)
        err_sys("write error");
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout)
{
    int n;
    if ((n == select(nfds, readfds, writefds, exceptfds, timeout)) == -1)
        err_sys("select error");
    return n;
}

int Pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask)
{
    int n;
    if ((n = pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask)) == -1)
        err_sys("pselect error");
    return n;
}

int Poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    int n;
    if ((n = poll(fds, nfds, timeout)) == -1)
        err_sys("poll error");
    return n;
}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t n;
    if ((n = recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) == -1)
        err_sys("recvfrom error");
    return n;
}

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen)
{
    ssize_t n;
    if ((n = sendto(sockfd, buf, len, flags, dest_addr, addrlen)) == -1)
        err_sys("sendto error");
    return n;
}