#ifndef ThreadSafeBoundedQueue
#define ThreadSafeBoundedQueue
#include "MyLock.h"
#define MAX 5
#include <iostream>
void testxx();

class TSQueue
{
private:
    MyLock lock;
    int items[MAX];
    int front = 0;
    int nextEmpty = 0;

public:
    TSQueue(/* args */);
    ~TSQueue();
    bool tryInsert(int item)
    {
        bool flag = false;
        lock.acquire();
        if ((nextEmpty - front) < MAX)
        {
            flag = true;
            items[nextEmpty % MAX] = item;
            nextEmpty++;
        }
        else
        {
            std::cout << "tryInsert failed !" << std::endl;
        }
        lock.release();
        return flag;
    }
    bool tryRemove(int *item)
    {
        bool flag = false;
        lock.acquire();
        if (front < nextEmpty)
        {
            flag = true;
            *item = items[front % MAX];
            front++;
        }
        else
        {
            std::cout << "tryRemove failed!" << std::endl;
        }
        lock.release();
        return flag;
    }
};

#endif