
#include "ThreadSafeBoundedQueue_FIFO.h"
#include <iostream>

pthread_cond_t *FIFOBBQ::genCond()
{
    pthread_cond_t *cond = new pthread_cond_t;
    pthread_cond_init(cond, NULL);

    return cond;
}

void FIFOBBQ::delCond(pthread_cond_t *cond)
{
    // std::cout << "delCond --" << std::endl;
    pthread_cond_destroy(cond);
    delete cond;
    // std::cout << "--- delCond" << std::endl;
}

FIFOBBQ::FIFOBBQ(/* args */)
{

    int res;
    res = pthread_mutex_init(&mtx, NULL);
    if (res != 0)
        std::cout << "FIFOBBQ mutex init failed!" << std::endl;
}

FIFOBBQ::~FIFOBBQ()
{
    int res;
    res = pthread_mutex_destroy(&mtx);
    if (res != 0)
        std::cout << "FIFOBBQ mutex destroy failed!" << std::endl;
}

bool FIFOBBQ::tryInsert(int item)
{
    bool flag = false;
    pthread_mutex_lock(&mtx);
    int myPosition = numInsertCalled++;
    pthread_cond_t *myCond = genCond();
    if ((nextEmpty - front) == MAX || myPosition > nextEmpty)
    {
        insert_list.push_back(myCond);

        while ((nextEmpty - front) == MAX || myPosition > nextEmpty)
        {
            std::cout << "tryInsert wait" << std::endl;

            pthread_cond_wait(myCond, &mtx);
        }
    }

    delCond(myCond);
    flag = true;
    items[nextEmpty % MAX] = item;
    nextEmpty++;

    pthread_cond_t *removeCond = remove_list.front();
    if (removeCond != nullptr)
    {
        remove_list.pop_front();
        pthread_cond_broadcast(removeCond);
    }

    pthread_mutex_unlock(&mtx);

    return flag;
}

bool FIFOBBQ::tryRemove(int *item)
{
    bool flag = false;
    pthread_mutex_lock(&mtx);

    int myPosition = numRemoveCalled++;
    pthread_cond_t *myCond = genCond();

    if ((nextEmpty - front) == 0 || myPosition > front)
    {
        remove_list.push_back(myCond);

        while ((nextEmpty - front) == 0 || myPosition > front)
        {
            std::cout << "tryRemove wait" << std::endl;
            pthread_cond_wait(myCond, &mtx);
        }
    }
    delCond(myCond);

    flag = true;
    *item = items[front % MAX];
    front++;

    pthread_cond_t *insertCond = insert_list.front();

    if (insertCond != nullptr)
    {
        insert_list.pop_front();
        pthread_cond_broadcast(insertCond);
    }

    pthread_mutex_unlock(&mtx);

    return flag;
}