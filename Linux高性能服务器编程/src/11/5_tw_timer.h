/**
 * 时间轮 tw_timer.h
 *
 */

#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <time.h>
#include <arpa/inet.h>
#include <iostream>
#define BUFFER_SIZE 64
class tw_timer;

struct client_data
{
    tw_timer *timer;
    char buf[BUFFER_SIZE];
    struct sockaddr_in address;
    int sockfd;
};

class tw_timer
{

public:
    tw_timer(int rot, int ts) : rotation{rot}, time_slot{ts} {}

public:
    int rotation;
    int time_slot;
    time_t expire;
    void (*cb_func)(client_data *) = nullptr;
    client_data *userdata;
    tw_timer *prev = nullptr;
    tw_timer *next = nullptr;
};

class time_wheel
{
private:
    tw_timer *head = nullptr;
    tw_timer *tail = nullptr;
    void add_timer(tw_timer *timer, tw_timer *lst_head);

public:
    ~time_wheel();
    void add_timer(tw_timer *);
    void ajust_timer(tw_timer *);
    void del_timer(tw_timer *);
    void tick();
};

time_wheel::~time_wheel()
{
    auto tmp = head;

    while (tmp)
    {
        std::cout << ":" << (void *)tmp << " start delete \n";
        head = tmp->next;
        delete tmp;
        tmp = head;
    }
}

void time_wheel::add_timer(tw_timer *timer, tw_timer *lst_head)
{
    auto prev = lst_head;
    auto tmp = prev->next;
    while (tmp)
    {
        if (timer->expire < tmp->expire)
        {
            prev->next = timer;
            timer->next = tmp;
            tmp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = tmp;
        tmp = prev->next;
    }
    if (!tmp)
    {
        prev->next = timer;
        timer->prev = prev;
        timer->next = nullptr;
        tail = timer;
    }
}
void time_wheel::add_timer(tw_timer *timer)
{
    if (!timer)
        return;
    if (!head)
    {
        head = tail = timer;
        return;
    }
    if (timer->expire < head->expire)
    {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }
    add_timer(timer, head);
}

void time_wheel::ajust_timer(tw_timer *timer)
{
    if (!timer)
        return;

    auto tmp = timer->next;
    if (!tmp || timer->expire < tmp->expire)
    {
        return;
    }
    if (timer == head)
    {
        head = head->next;
        head->prev = nullptr;
        tmp->next = nullptr;
        add_timer(timer, head);
    }
    else
    {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer, head);
    }
}
void time_wheel::del_timer(tw_timer *timer)
{
    if (!timer)
    {
        return;
    }
    if (timer == head && timer == tail)
    {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return;
    }
    if (head == timer)
    {
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return;
    }
    if (tail == timer)
    {
        tail = tail->prev;
        tail->next = nullptr;
        delete timer;
        return;
    }
}
void time_wheel::tick()
{

    if (!head)
        return;
    std::cout << "timer tick \n";

    time_t cur = time(NULL);
    auto tmp = head;

    while (tmp)
    {
        if (cur < tmp->expire)
            break;
        tmp->cb_func(tmp->userdata);

        head = tmp->next;
        if (head)
            head->prev = nullptr;
        delete tmp;
        tmp = head;
    }
}
#endif
