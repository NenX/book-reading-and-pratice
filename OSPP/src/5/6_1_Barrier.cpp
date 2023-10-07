// Thread-Safe Bounded Queue
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "Barrier.h"
#include "MyLock.h"
static Barrier b(3);
static MyLock lock;
static void *threadFunc(void *arg)
{
    long i = reinterpret_cast<long>(arg);
    int remove_n = 0;
    b.checkin();
    lock.acquire();
    std::cout << "checkin 1:" << i << "------------" << std::endl;
    lock.release();
    if (i == 2)
    {

        sleep(2);
        std::cout << "sleep============" << std::endl;
    }
    b.checkin();

    lock.acquire();

    std::cout << "checkin 2:" << i << std::endl;

    lock.release();
}

int main(int argc, char const *argv[])
{
    std::cout << "++++" << std::endl;

    pthread_t t1, t2, t3, t4, t5;
    int res;
    res = pthread_create(&t1, NULL, threadFunc, (void *)0);
    res = pthread_create(&t2, NULL, threadFunc, (void *)1);
    res = pthread_create(&t3, NULL, threadFunc, (void *)2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    std::cout << "-----" << std::endl;
    return 0;
}
