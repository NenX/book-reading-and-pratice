#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include "locker.h"
#include <pthread.h>
#include <list>
#include <assert.h>
#include <exception>
template <typename T>
class threadpool
{
private:
    int m_thread_number;
    int m_max_requests;
    pthread_t *m_threads;
    std::list<T *> m_workqueue;
    locker m_queuelocker;
    sem m_queuestat;
    bool m_stop;

    static void *worker(void *);
    void run();

public:
    threadpool(int thread_number = 8, int max_requests = 1000);
    ~threadpool();
    bool append(T *request);
};
template <typename T>
threadpool<T>::threadpool(int thread_number, int max_requests) : m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL), m_stop(false)
{
    assert(thread_number > 0);
    assert(max_requests > 0);

    m_threads = new pthread_t[thread_number];
    assert(m_threads);

    for (size_t i = 0; i < thread_number; i++)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}
template <typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
    m_stop = true;
}

template <typename T>
bool threadpool<T>::append(T *request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template <typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool<T> *t = (threadpool<T> *)arg;
    t->run();
    return t;
}

template <typename T>
void threadpool<T>::run()
{
    while (!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        if (request)
        {
            request.process();
        }
    }
}
#endif