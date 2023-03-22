#include "unp.h"
#include "unp_error.h"

#ifndef __unp_wrapper_h
#define __unp_wrapper_h


typedef void sigfunc(int);

sigfunc *Signal(int signo, sigfunc * func);

int Socket(int family, int type, int protocol);

void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd, int backlog);

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void Close(int fd);

void Getsockname(int sockfd, struct sockaddr *sa, socklen_t *salenptr);

void Getpeername(int sockfd, struct sockaddr *sa, socklen_t *salenptr);

void Setsockopt(int sockfd, int level, int optname,
                const void *optval, socklen_t optlen);

void Getsockopt(int sockfd, int level, int optname,
                void *optval, socklen_t *optlen);

void Shutdown(int fd, int how);

const char *Inet_ntop(int family, const void *src, char *dst, socklen_t size);

void Inet_pton(int family, const char *src, void *dst);

/**
 *  unix c function wrapper
 */

void *Calloc(size_t n, size_t size);

void *Malloc(size_t size);

pid_t Fork();

ssize_t Read(int fd, void *ptr, size_t nbytes);

void Write(int fd, void *ptr, size_t nbytes);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);

int Pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask);

int Poll(struct pollfd *fds, nfds_t nfds, int timeout);

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);

#endif
