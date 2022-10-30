#ifndef PROCESSPOLL_H
#define PROCESSPOLL_H

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "tlpi_hdr.h"
static int sig_pipefd[2];

static int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;

    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

static void addfd(int epollfd, int fd)
{
    struct epoll_event e;
    e.data.fd = fd;
    e.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &e);
    setnonblocking(fd);
}
static void removefd(int epollfd, int fd)
{

    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}
static void sig_handler(int sig)
{
    int save_errno = errno;

    send(sig_pipefd[1], (char *)&sig, 1, 0);
    errno = save_errno;
}

static void addsig(int sig, void(handler)(int), bool restart = false)
{
    struct sigaction sa;
    bzero(&sa, sizeof(sa));

    sa.sa_handler = handler;

    if (restart)
    {
        sa.sa_flags |= SA_RESTART;
    }

    sigfillset(&sa.sa_mask);

    assert(sigaction(sig, &sa, NULL) != -1);
}
class process
{
private:
public:
    pid_t m_pid;
    int pipefd[2];
    process() : m_pid(-1) {}
};

template <typename T>
class processpool
{
private:
    processpool(int listenfd, int process_number);

    void setup_sig_pipe();
    void run_parent();
    void run_child();

    static const int MAX_PROCESS_NUMBER = 16;
    static const int USER_PER_PROCESS = 65535;
    static const int MAX_EVENT_NUMBER = 10000;

    int m_process_number;
    int m_idx;
    int m_epollfd;
    int m_listenfd;
    int m_stop;

    process *m_sub_process;
    static processpool<T> *m_instance;

public:
    static processpool<T> *create(int listenfd, int process_number);
    void run();

    ~processpool();
};

template <typename T>
processpool<T> *processpool<T>::m_instance = NULL;

template <typename T>
processpool<T> *processpool<T>::create(int listenfd, int process_number)
{
    if (!m_instance)
    {
        m_instance = new processpool<T>(listenfd, process_number);
    }
    return m_instance;
}

template <typename T>
processpool<T>::processpool(int listenfd, int process_number)
    : m_process_number(process_number), m_listenfd(listenfd), m_stop(false), m_idx(-1)
{
    assert(process_number > 0);
    assert(process_number <= MAX_PROCESS_NUMBER);

    m_sub_process = new process[process_number];
    assert(m_sub_process);

    for (size_t i = 0; i < process_number; i++)
    {
        process *target = m_sub_process + i;

        int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, target->pipefd);
        assert(ret == 0);

        target->m_pid = fork();
        assert(target->m_pid >= 0);

        if (target->m_pid > 0)
        {
            close(target->pipefd[1]);
            continue;
        }
        else
        {
            close(target->pipefd[0]);
            m_idx = i;
            break;
        }
    }
}

template <typename T>
processpool<T>::~processpool()
{
    delete[] m_sub_process;
}

template <typename T>
void processpool<T>::setup_sig_pipe()
{
    m_epollfd = epoll_create(5);
    assert(m_epollfd != 1);

    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sig_pipefd);
    assert(ret != 1);

    setnonblocking(sig_pipefd[1]);
    addfd(m_epollfd, sig_pipefd[0]);

    addsig(SIGCHLD, sig_handler);
    addsig(SIGINT, sig_handler);
    addsig(SIGTERM, sig_handler);
    addsig(SIGPIPE, SIG_IGN);
}

template <typename T>
void processpool<T>::run()
{
    setup_sig_pipe();
    if (m_idx == -1)
    {
        run_parent();
    }
    else
    {
        run_child();
    }
}

template <typename T>
void processpool<T>::run_parent()
{
    addfd(m_epollfd, m_listenfd);

    struct epoll_event events[MAX_EVENT_NUMBER];

    int sub_child_counter = 0;
    int new_conn = 9527;
    while (!m_stop)
    {
        int n = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            printf("epoll failed\n");
            break;
        }
        for (size_t i = 0; i < n; i++)
        {
            int sockfd = events[i].data.fd;
            bool isIn = events[i].events & EPOLLIN;

            if (sockfd == m_listenfd && isIn)
            {

                int num = 0;

                int n = sub_child_counter;

                do
                {
                    if (m_sub_process[n].m_pid != -1)
                    {
                        break;
                    }
                    n = (n + 1) % m_process_number;
                } while (n != sub_child_counter);

                if (m_sub_process[n].m_pid == -1)
                {
                    printf("GG %d  %d:%d\n", m_sub_process[n].m_pid, sockfd, m_listenfd);

                    break;
                }

                int ret = send(m_sub_process[n].pipefd[0], (char *)&new_conn, sizeof(new_conn), 0);
                printf("parent: send request to child(%d,%d,%d)\n", n, m_sub_process[n].m_pid, m_sub_process[n].pipefd[0], m_sub_process[n].pipefd[1]);
            }
            else if (sockfd == sig_pipefd[0] && isIn)
            {
                char signals[1024];

                int n = recv(sig_pipefd[0], signals, 1024, 0);

                if (n <= 0)
                {
                    continue;
                }
                for (size_t i = 0; i < n; i++)
                {
                    m_stop = true;
                    int pid = 0;

                    switch (signals[i])
                    {
                    case SIGINT:
                    case SIGTERM:

                        for (size_t i = 0; i < m_process_number; i++)
                        {
                            if (m_sub_process[i].m_pid != -1)
                            {
                                kill(m_sub_process[i].m_pid, SIGTERM);
                            }
                        }

                        break;
                    case SIGCHLD:

                        while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
                        {
                            for (size_t i = 0; i < m_process_number; i++)
                            {
                                if (m_sub_process[i].m_pid == pid)
                                {
                                    m_sub_process[i].m_pid = -1;

                                    close(m_sub_process[i].pipefd[0]);
                                }
                                if (m_sub_process[i].m_pid != -1)
                                {
                                    m_stop = false;
                                }
                            }
                        }

                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                continue;
            }
        }
    }

    close(m_epollfd);
    close(sig_pipefd[0]);
    close(sig_pipefd[1]);
}

template <typename T>
void processpool<T>::run_child()
{
    int pipefd = m_sub_process[m_idx].pipefd[1];

    addfd(m_epollfd, pipefd);

    struct epoll_event events[MAX_EVENT_NUMBER];
    T *users = new T[USER_PER_PROCESS];

    assert(users);

    while (!m_stop)
    {
        int numbers = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);

        if ((numbers < 0 && errno == EINTR) || numbers == 0)
        {
            continue;
        }
        for (size_t i = 0; i < numbers; i++)
        {

            int sockfd = events[i].data.fd;
            int sock_events = events[i].events;
            bool isIn = sock_events & EPOLLIN;

            if ((sockfd == pipefd) && isIn)
            {
                int client = 0;
                int ret = recv(pipefd, (char *)&client, sizeof(client), 0);
                printf("child recv:%d,%d\n", client, client);
        

                if (ret <= 0)
                {
                    continue;
                }
                struct sockaddr_in client_address;
                socklen_t client_len = sizeof(client_address);
                int confd = accept(m_listenfd, (struct sockaddr *)&client_address, &client_len);
                if (confd <= 0)
                {
                    printf("epoll failed\n");
                    continue;
                }
                addfd(m_epollfd, confd);
                T *u = users + confd;
                u->init(m_epollfd, confd, client_address);
            }
            else if (sockfd == sig_pipefd[0] && isIn)

            {
                char signals[1024];

                int n = recv(sig_pipefd[0], &signals, sizeof(signals), 0);

                if (n <= 0)
                {
                    continue;
                }
                for (size_t i = 0; i < n; i++)
                {
                    switch (signals[i])
                    {
                    case SIGINT:
                    case SIGTERM:
                        m_stop = true;

                        break;
                    case SIGCHLD:
                        while (waitpid(-1, NULL, WNOHANG) != -1)
                        {
                            continue;
                        }

                        break;
                    default:
                        break;
                    }
                }
            }
            else if (isIn)
            {
                users[sockfd].process();
            }
            else
            {
                continue;
            }
        }
    }
    close(sig_pipefd[0]);
    close(sig_pipefd[1]);
    close(pipefd);
    close(m_epollfd);
}
#endif