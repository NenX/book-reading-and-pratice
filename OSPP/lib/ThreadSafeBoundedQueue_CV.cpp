
#include "ThreadSafeBoundedQueue_CV.h"
#include <iostream>

BBQ::BBQ(/* args */)
{
    int res;
    res = pthread_mutex_init(&mtx, NULL);
    if (res != 0)
        std::cout << "BBQ mutex init failed!" << std::endl;
    res = pthread_cond_init(&itemAddedCond, NULL);
    if (res != 0)
        std::cout << "BBQ itemAddedCond init failed!" << std::endl;
    res = pthread_cond_init(&itemRemovedCond, NULL);
    if (res != 0)
        std::cout << "BBQ itemRemovedCond init failed!" << std::endl;
}

BBQ::~BBQ()
{
    int res;
    res = pthread_mutex_destroy(&mtx);
    if (res != 0)
        std::cout << "BBQ mutex destroy failed!" << std::endl;
    res = pthread_cond_destroy(&itemAddedCond);
    if (res != 0)
        std::cout << "BBQ itemAddedCond destroy failed!" << std::endl;
    res = pthread_cond_destroy(&itemRemovedCond);
    if (res != 0)
        std::cout << "BBQ itemRemovedCond destroy failed!" << std::endl;
}

bool BBQ::tryInsert(int item)
{
    bool flag = false;
    pthread_mutex_lock(&mtx);

    while ((nextEmpty - front) == MAX)
    {
        std::cout << "tryInsert wait" << std::endl;

        pthread_cond_wait(&itemAddedCond, &mtx);
    }

    flag = true;
    items[nextEmpty % MAX] = item;
    nextEmpty++;

    pthread_cond_broadcast(&itemRemovedCond);
    pthread_mutex_unlock(&mtx);

    return flag;
}

bool BBQ::tryRemove(int *item)
{
    bool flag = false;
    pthread_mutex_lock(&mtx);

    while ((nextEmpty - front) == 0)
    {
        std::cout << "tryRemove wait" << std::endl;
        pthread_cond_wait(&itemRemovedCond, &mtx);
    }

    flag = true;
    *item = items[front % MAX];
    front++;
    pthread_cond_broadcast(&itemAddedCond);

    pthread_mutex_unlock(&mtx);

    return flag;
}