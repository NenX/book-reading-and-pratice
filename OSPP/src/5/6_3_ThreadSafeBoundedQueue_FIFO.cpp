// Thread-Safe Bounded Queue
#include <iostream>
#include <pthread.h>
#include "ThreadSafeBoundedQueue_FIFO.h"
FIFOBBQ q;
static void *threadFunc(void *arg)
{
    long i = reinterpret_cast<long>(arg);
    int remove_n = 0;
    std::cout << "num is :" << i << std::endl;
    if (i == 0)
    {
        q.tryInsert(1);
        std::cout << "insert" << 1 << std::endl;

        q.tryInsert(2);
        std::cout << "insert" << 2 << std::endl;

        q.tryInsert(3);
        std::cout << "insert" << 3 << std::endl;

        q.tryInsert(4);
        std::cout << "insert" << 4 << std::endl;

        q.tryInsert(5);
        std::cout << "insert" << 5 << std::endl;

        q.tryInsert(6);
        std::cout << "insert" << 6 << std::endl;

    }
    else
    {
        q.tryRemove(&remove_n);
        std::cout << "remove" << remove_n << std::endl;

        q.tryRemove(&remove_n);
        std::cout << "remove" << remove_n << std::endl;

        q.tryRemove(&remove_n);
        std::cout << "remove" << remove_n << std::endl;

        q.tryRemove(&remove_n);
        std::cout << "remove" << remove_n << std::endl;

        q.tryRemove(&remove_n);
        std::cout << "remove" << remove_n << std::endl;

        q.tryRemove(&remove_n);
        std::cout << "remove" << remove_n << std::endl;
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
    return 0;
}
