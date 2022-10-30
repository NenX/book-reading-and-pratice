#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <wait.h>
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd)
{
    struct epoll_event e;
    e.data.fd = fd;
    e.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &e);
}

void main()
{
    int pipefd[2];
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd);

    assert(ret == 0);

    int pid = fork();

    assert(pid >= 0);

    int m_close = 0;
    // child
    if (pid == 0)
    {

        int epollfd = epoll_create(5);

        assert(epollfd >= 0);

        struct epoll_event events[1024];
        close(pipefd[0]);
        addfd(epollfd, pipefd[1]);
        while (!m_close)
        {
            printf("i am child\n");

            int n = epoll_wait(epollfd, events, 1024, -1);

            if (n < 0 && errno != EINTR)
            {
                close(epollfd);
                close(pipefd[1]);

                return;
            }

            for (size_t i = 0; i < n; i++)
            {
                struct epoll_event *e = events + i;
                if (e->data.fd == pipefd[1] && (e->events & EPOLLIN))
                {
                    char msg[1024];
                    int ret = recv(pipefd[1], msg, 1024, '\0');

                    if (ret >= 0)
                    {
                        printf("child: recv %s\n", msg);
                        m_close = 1;
                        continue;
                    }
                }
            }
        }
        printf("i am child after while\n");
    }
    else
    {
        printf("i am parent\n");

        close(pipefd[1]);

        sleep(2);
        char *msg = "wo shi baba\n";
        int n = send(pipefd[0], msg, sizeof(msg), 0);
        printf("i am parent, after send %d\n", n);

        int pid = 0;
        while ((pid = waitpid(-1, NULL, 0)) != -1)
        {
            printf("parent: close %d\n", pid);
        }
    }
}