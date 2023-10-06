// Thread-Safe Bounded Queue
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "RWLock.h"
RWLock lock;
static void *threadFunc(void *arg)
{
    long i = reinterpret_cast<long>(arg);
    int remove_n = 0;
    std::cout << "num is :" << i << std::endl;
    if (i == 0)
    {
        lock.startRead();

        std::cout << "read ~~~~~~~~~~~~~~" << std::endl;
        sleep(2);
        lock.doneRead();
    }
    else
    {
        lock.startWrite();
        std::cout << "write ~~~~~~~~~~~~~" << std::endl;
        sleep(2);

        lock.doneWrite();
    }
}

int main(int argc, char const *argv[])
{
    std::cout << "++++" << std::endl;

    pthread_t t1, t2, t3, t4, t5;
    int res;
    res = pthread_create(&t1, NULL, threadFunc, (void *)0);
    res = pthread_create(&t2, NULL, threadFunc, (void *)0);

    res = pthread_create(&t3, NULL, threadFunc, (void *)1);
    res = pthread_create(&t4, NULL, threadFunc, (void *)1);
    res = pthread_create(&t5, NULL, threadFunc, (void *)0);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    std::cout << "-----" << std::endl;
    return 0;
}
