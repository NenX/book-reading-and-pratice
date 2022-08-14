/**
 * 关闭非活动连接
 *
 */

#include "2_lst_timer.h"
#include <fcntl.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 5

using std::cout;

static int pipefd[2];
static sort_timer_lst timer_lst;
static int epollfd = 0;

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}
void sig_handler(int sig)
{
    int save_errno = errno;

    int msg = sig;

    send(pipefd[1], (char *)&msg, 1, 0);

    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;

    bzero(&sa, sizeof(sa));

    sa.sa_flags |= SA_RESTART;
    sa.sa_handler = sig_handler;
    sigfillset(&sa.sa_mask);
    int ret = sigaction(sig, &sa, NULL);
    assert(ret != -1);
}

void timer_handler()
{
    timer_lst.tick();

    alarm(TIMESLOT);
}

void cb_fn(client_data *user_data)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);

    assert(user_data);
    close(user_data->sockfd);
    cout << "close fd:" << user_data->sockfd << "\n";
}

int main(int argc, char const *argv[])
{
    const char *ip;
    int port;
    if (argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        ip = "127.0.0.1";
        port = 1234;
    }
    else
    {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(struct sockaddr));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];

    epollfd = epoll_create(5);
    assert(epollfd != -1);

    addfd(epollfd, listenfd);

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);

    setnonblocking(pipefd[1]);
    addfd(epollfd, pipefd[0]);

    addsig(SIGALRM);
    addsig(SIGTERM);

    bool stop_server = false;
    bool timeout = false;

    auto users = new client_data[FD_LIMIT];

    alarm(TIMESLOT);

    while (!stop_server)
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            cout << "epoll failed\n";
            break;
        }
        for (size_t i = 0; i < number; i++)
        {
            auto target_event = events[i];
            auto target_fd = target_event.data.fd;
            if (target_fd == listenfd)
            {
                sockaddr_in client_address;
                socklen_t client_address_len = sizeof(client_address);
                int connfd = accept(listenfd, (sockaddr *)&client_address, &client_address_len);
                if (connfd == -1)
                    continue;

                auto target_user = users + connfd;
                auto timer = new util_timer;
                auto cur = time(NULL);

                target_user->address = client_address;
                target_user->sockfd = connfd;
                target_user->timer = timer;

                timer->userdata = target_user;
                timer->cb_func = cb_fn;
                timer->expire = cur + 3 * TIMESLOT;

                timer_lst.add_timer(timer);
            }
            else if (target_fd == pipefd[0] && target_event.events & EPOLLIN)
            {
                char signals[1024];

                int ret = recv(target_fd, signals, sizeof(signals), 0);
                if (ret > 0)
                {
                    for (size_t i = 0; i < ret; i++)
                    {
                        switch (signals[i])
                        {
                        case SIGTERM:
                            stop_server = true;
                            break;
                        case SIGALRM:
                            timeout = true;
                            break;

                        default:
                            break;
                        }
                    }
                }
            }
            else if (target_event.events & EPOLLIN)
            {
                auto target_user = users + target_fd;
                auto timer = target_user->timer;
                int ret = recv(target_fd, target_user->buf, BUFFER_SIZE - 1, 0);
                cout << "read" << ret << " bytes of data from " << target_fd;
                if (ret < 0)
                {
                    if (errno != EAGAIN)
                    {
                        cb_fn(target_user);
                        timer_lst.del_timer(timer);
                    }
                }
                else if (ret = 0)
                {
                    cb_fn(target_user);
                    timer_lst.del_timer(timer);
                }
                else
                {
                    timer->expire = time(NULL) + 3 * TIMESLOT;
                    timer_lst.ajust_timer(timer);
                }
            }
            else
            {
                /* code */
            }
        }

        if (timeout)
        {
            timer_handler();
            timeout = false;
        }
    }

    close(listenfd);
    close(pipefd[0]);
    close(pipefd[1]);
    delete[] users;

    return 0;
}
