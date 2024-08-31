#include <iostream>
#include "tlpi_hdr.h"
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>
#include <poll.h>
#include <sys/epoll.h>
#include <pthread.h>
#include "locker.h"
#define THREAD_MAX 5
cond c;
void *worker(void *x)
{
    c.wait();
    pid_t id = gettid();
    std::cout << "thread id:" << id << std::endl;
    sleep(1);
}

pthread_t *thread_ins;

void thread_init()
{
    thread_ins = (pthread_t *)malloc(sizeof(pthread_t) * THREAD_MAX);

    for (size_t i = 0; i < THREAD_MAX; i++)
    {
        pthread_t *ins = thread_ins + i;
        ssize_t ret = pthread_create(ins, NULL, worker, NULL);

        if (ret == -1)
            err_exit("pthread_create");
    }
}
void epoll_add_event(ssize_t epoll_id, ssize_t fd)
{
    epoll_event e;
    e.data.fd = fd;
    e.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(epoll_id, EPOLL_CTL_ADD, fd, &e);
    if (ret == -1)
        err_exit("epoll_add_event");
}

int main(int argc, char const *argv[])
{
    char client_read_buf[FILENAME_MAX];
    ssize_t ret, client_fd, client_addr_len, epoll_id;
    struct sockaddr_in server_addr, client_addr;
    epoll_event events[FILENAME_MAX];

    epoll_event *tmp_e;
    thread_init();
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8022);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        err_exit("socket");

    epoll_id = epoll_create(5);
    if (epoll_id == -1)
        err_exit("epoll_create");

    epoll_add_event(epoll_id, server_fd);

    ret = bind(server_fd, (sockaddr *)&server_addr, (socklen_t)sizeof(server_addr));
    if (ret == -1)
        err_exit("bind");

    ret = listen(server_fd, 5);
    if (ret == -1)
        err_exit("listen");

    while (1)
    {
        std::cout << "epoll waiting..." << std::endl;

        ret = epoll_wait(epoll_id, events, FILENAME_MAX, -1);

        std::cout << "epoll_wait..." << ret << std::endl;

        if (ret == -1)
            err_exit("epoll_wait");
        for (size_t i = 0; i < ret; i++)
        {
            tmp_e = events + i;
            if (tmp_e->data.fd == server_fd)
            {
                client_fd = accept(server_fd, (sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
                if (client_fd == -1)
                    err_exit("accept");

                epoll_add_event(epoll_id, client_fd);
            }
            else if (tmp_e->events & EPOLLIN)

            {

                ret = recv(tmp_e->data.fd, client_read_buf, FILENAME_MAX, 0);
                if (client_fd == -1)
                    err_exit("recv");

                std::cout << "?x:" << client_read_buf << std::endl;

                ret = send(client_fd, "sb!\n", 4, 0);
                if (client_fd == -1)
                    err_exit("recv");
                close(client_fd);
            }

            // else if (tmp_e->events & EPOLLIN)
            // {
            //     /* code */
            // }
        }
    }

    return 0;
}
