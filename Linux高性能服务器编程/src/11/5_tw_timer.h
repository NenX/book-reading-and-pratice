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
    static const int N = 60;
    static const int SI = 1;
    tw_timer *slots[N];
    int cur_slot;

    tw_timer *head = nullptr;
    tw_timer *tail = nullptr;

public:
    time_wheel();
    ~time_wheel();
    void add_timer(int);
    void del_timer(tw_timer *);
    void tick();
};

time_wheel::time_wheel()
{
    for (size_t i = 0; i < N; i++)
    {
        slots[i] = nullptr;
    }
}

time_wheel::~time_wheel()
{

    for (size_t i = 0; i < N; i++)
    {
        auto tmp = slots[i];

        while (tmp)
        {
            std::cout << ":" << (void *)tmp << " start delete \n";
            slots[i] = tmp->next;
            delete tmp;
            tmp = slots[i];
        }
    }
}

void time_wheel::add_timer(int timeout)
{

    int ticks = timeout < SI ? SI : timeout / SI;
    int rotation = ticks / N;
    int ts = (cur_slot + ticks % N) % N;

    auto time = new tw_timer(rotation, ts);

    if (!slots[ts])
    {
        slots[ts] = time;
    }
    else
    {
        slots[ts]->prev = time;
        time->next = slots[ts];
        slots[ts] = time;
    }
}

void time_wheel::del_timer(tw_timer *timer)
{
    if (!timer)
    {
        return;
    }
    auto ts = timer->time_slot;
    if (slots[ts] == timer)
    {
        slots[ts] = timer->next;
        if (slots[ts])
        {
            slots[ts]->prev = nullptr;
        }
    }
    else
    {
        timer->prev->next = timer->next;
        if (timer->next)
        {
            timer->next->prev = timer->prev;
        }
    }
    delete timer;
}
void time_wheel::tick()
{

    {
        auto head = slots[cur_slot];
        if (!head)
            return;
        std::cout << "timer tick \n";

        time_t cur = time(NULL);
        auto tmp = head;

        while (tmp)
        {
            if (tmp->rotation > 0)
            {
                tmp->rotation--;
                tmp = tmp->next;
                continue;
            }
            tmp->cb_func(tmp->userdata);

            if (tmp == head)
            {
                slots[cur_slot] = tmp->next;
                if (slots[cur_slot])
                    slots[cur_slot]->prev = nullptr;
                tmp = slots[cur_slot];
            }
            else
            {
                tmp->prev->next = tmp->next;
                if (tmp->next)
                {
                    tmp->next->prev = tmp->prev;
                }
                tmp = tmp->next;

            }
            delete tmp;
        }
        cur_slot = ++cur_slot % N;
    }
#endif
