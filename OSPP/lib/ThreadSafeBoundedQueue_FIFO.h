#ifndef ThreadSafeBoundedQueue
#define ThreadSafeBoundedQueue
#include <pthread.h>
#define MAX 5
#include <iostream>
#include <list>
class FIFOBBQ
{
private:
    pthread_mutex_t mtx;
    pthread_cond_t itemAddedCond;
    pthread_cond_t itemRemovedCond;
    int items[MAX];
    int front = 0;
    int nextEmpty = 0;
    std::list<pthread_cond_t *> insert_list;
    std::list<pthread_cond_t *> remove_list;
    int numInsertCalled = 0;
    int numRemoveCalled = 0;

    pthread_cond_t *genCond();
    void delCond(pthread_cond_t *);

public:
    FIFOBBQ(/* args */);
    ~FIFOBBQ();
    bool tryInsert(int item);
    bool tryRemove(int *item);
};

#endif