#ifndef Barrier_H
#define Barrier_H
#include <pthread.h>
#include <iostream>
class Barrier
{
private:
    pthread_mutex_t mtx;
    pthread_cond_t allCheckedInCond;
    pthread_cond_t allLeavingCond;

    int numThreads = 0;
    int numEntered = 0;
    int numLeaving = 0;

public:
    Barrier(int);
    ~Barrier();
    void checkin();
};

#endif