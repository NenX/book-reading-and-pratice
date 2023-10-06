#include "RWLock.h"

bool RWLock::readShouldWait()
{

    return waitingWriters > 0 || activeWriters > 0;
}

bool RWLock::writeShouldWait()
{
    return activeWriters > 0 || activeReaders > 0;
}

RWLock::RWLock(/* args */)
{
    int res;
    res = pthread_mutex_init(&mtx, NULL);
    if (res != 0)
        std::cout << "RWLock mutex init failed!" << std::endl;
    res = pthread_cond_init(&readCond, NULL);
    if (res != 0)
        std::cout << "RWLock itemAddedCond init failed!" << std::endl;
    res = pthread_cond_init(&writeCond, NULL);
    if (res != 0)
        std::cout << "RWLock itemRemovedCond init failed!" << std::endl;
}

RWLock::~RWLock()
{
    int res;
    res = pthread_mutex_destroy(&mtx);
    if (res != 0)
        std::cout << "RWLock mutex destroy failed!" << std::endl;
    res = pthread_cond_destroy(&readCond);
    if (res != 0)
        std::cout << "RWLock itemAddedCond destroy failed!" << std::endl;
    res = pthread_cond_destroy(&writeCond);
    if (res != 0)
        std::cout << "RWLock itemRemovedCond destroy failed!" << std::endl;
}

void RWLock::startRead()
{
    pthread_mutex_lock(&mtx);
    waitingReaders++;
    while (readShouldWait())
    {
        pthread_cond_wait(&readCond, &mtx);
    }
    waitingReaders--;
    activeReaders++;

    pthread_mutex_unlock(&mtx);
}

void RWLock::doneRead()
{
    pthread_mutex_lock(&mtx);
    activeReaders--;
    if (activeReaders == 0 && waitingWriters > 0)
    {
        pthread_cond_signal(&writeCond);
    }
    pthread_mutex_unlock(&mtx);
}

void RWLock::startWrite()
{
    pthread_mutex_lock(&mtx);
    waitingWriters++;
    while (writeShouldWait())
    {
        pthread_cond_wait(&writeCond, &mtx);
    }
    waitingWriters--;
    activeWriters++;

    pthread_mutex_unlock(&mtx);
}

void RWLock::doneWrite()
{
    pthread_mutex_lock(&mtx);
    activeWriters--;
    if (waitingWriters > 0)
    {
        pthread_cond_signal(&writeCond);
    }
    else if (waitingReaders > 0)
    {
        pthread_cond_broadcast(&readCond);
    }
    pthread_mutex_unlock(&mtx);
}
