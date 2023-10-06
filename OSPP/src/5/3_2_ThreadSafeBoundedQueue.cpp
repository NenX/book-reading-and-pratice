// Thread-Safe Bounded Queue
#include <iostream>
#include <pthread.h>
#include "ThreadSafeBoundedQueue.h"
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
TSQueue q;
static void *threadFunc(void *arg)
{
    long i = reinterpret_cast<long>(arg);
    int remove_n = 0;
    std::cout << "num is :" << i << std::endl;
    if (i == 0)
    {
        q.tryInsert(1);
        q.tryInsert(1);
        q.tryInsert(1);
        q.tryInsert(1);
        q.tryInsert(1);
        q.tryInsert(1);
    }
    else
    {
        q.tryRemove(&remove_n);
        q.tryRemove(&remove_n);
        q.tryRemove(&remove_n);
        q.tryRemove(&remove_n);
        q.tryRemove(&remove_n);
        q.tryRemove(&remove_n);
    }
}

int main(int argc, char const *argv[])
{
    std::cout << "++++" << std::endl;

    pthread_t t1, t2;
    int res;
    res = pthread_create(&t1, NULL, threadFunc, (void *)0);
    if (res != 0)
    {
        std::cout << "fuck1" << std::endl;
    }
    res = pthread_create(&t2, NULL, threadFunc, (void *)1);
    if (res != 0)
    {
        std::cout << "fuck2" << std::endl;
    }
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    std::cout << "-----" << std::endl;
    testxx();
    return 0;
}
