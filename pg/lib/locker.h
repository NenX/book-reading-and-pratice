#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
private:
    sem_t m_sem;

public:
    sem();
    ~sem();
    bool wait();
    bool post();
};

sem::sem()
{
    if (sem_init(&m_sem, 0, 0) != 0)
    {
        throw std::exception();
    }
}

sem::~sem()
{
    sem_destroy(&m_sem);
}
bool sem::wait()
{
    return sem_wait(&m_sem) == 0;
}
bool sem::post()
{
    return sem_post(&m_sem) == 0;
}

class locker
{
private:
    pthread_mutex_t m_mutex;

public:
    locker(/* args */);
    ~locker();
    bool lock();
    bool unlock();
};

locker::locker(/* args */)
{
    if (pthread_mutex_init(&m_mutex, NULL) != 0)
    {
        throw new std::exception();
    }
}

locker::~locker()
{
}
bool locker::lock()
{
    return pthread_mutex_lock(&m_mutex) == 0;
}

bool locker::unlock()
{
    return pthread_mutex_unlock(&m_mutex) == 0;
}

class cond
{
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

public:
    cond();
    ~cond();
    bool wait();
    bool signal();
};

cond::cond()
{
    if (pthread_mutex_init(&m_mutex, NULL) != 0)
    {
        throw new std::exception();
    }
    if (pthread_cond_init(&m_cond, NULL) != 0)
    {
        pthread_mutex_destroy(&m_mutex);
        throw new std::exception();
    }
}

cond::~cond()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}
bool cond::wait()
{
    int ret = 0;
    pthread_mutex_lock(&m_mutex);
    ret = pthread_cond_wait(&m_cond, &m_mutex);
    pthread_mutex_unlock(&m_mutex);
    return ret;
}

bool cond::signal()
{
    return pthread_cond_signal(&m_cond) == 0;
}
#endif