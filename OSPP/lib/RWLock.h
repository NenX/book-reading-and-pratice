#ifndef RWLOCK_H
#define RWLOCK_H
#include <pthread.h>
#include <iostream>
class RWLock
{
private:
    pthread_mutex_t mtx;
    pthread_cond_t readCond;
    pthread_cond_t writeCond;

    int activeReaders = 0;
    int waitingReaders = 0;
    int activeWriters = 0;
    int waitingWriters = 0;

    bool readShouldWait();
    bool writeShouldWait();

public:
    RWLock(/* args */);
    ~RWLock();
    void startRead();
    void doneRead();
    void startWrite();
    void doneWrite();
};

#endif