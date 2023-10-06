#ifndef ThreadSafeBoundedQueue
#define ThreadSafeBoundedQueue
#include <pthread.h>
#define MAX 5
#include <iostream>

class BBQ
{
private:
    pthread_mutex_t mtx;
    pthread_cond_t itemAddedCond;
    pthread_cond_t itemRemovedCond;
    int items[MAX];
    int front = 0;
    int nextEmpty = 0;

public:
    BBQ(/* args */);
    ~BBQ();
    bool tryInsert(int item);
    bool tryRemove(int *item);
};

#endif