
#include "ThreadSafeBoundedQueue.h"
#include <iostream>
#include "MyLock.h"
void testxx()
{
    MyLock lock;
    lock.acquire();
    std::cout << "testxx 22221" << std::endl;
    lock.release();
}

TSQueue::TSQueue(/* args */)
{
}

TSQueue::~TSQueue()
{
}
