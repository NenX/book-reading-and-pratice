#ifndef MyLock_H
#define MyLock_H
#include <pthread.h>
#include <iostream>

class MyLock
{
private:
    pthread_mutex_t mtx;

public:
    MyLock(/* args */)
    {
        int res = pthread_mutex_init(&mtx, NULL);
        if (res != 0)
            std::cout << "MyLock init failed!" << std::endl;
    };
    ~MyLock()
    {
        int res = pthread_mutex_init(&mtx, NULL);
        if (res != 0)
            std::cout << "MyLock init failed!" << std::endl;
    };
    bool acquire()
    {
        int res = pthread_mutex_lock(&mtx);
        if (res != 0)
            std::cout << "MyLock acquire failed!" << std::endl;

        // std::cout << "MyLock acquire !" << std::endl;
    }
    bool release()
    {
        int res = pthread_mutex_unlock(&mtx);
        if (res != 0)
            std::cout << "MyLock release failed!" << std::endl;
        // std::cout << "MyLock release !" << std::endl;
    }
};

#endif