// Thread-Safe Bounded Queue
#include <iostream>
#include <list>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define MAX 200000000
pthread_spinlock_t s;
class Item
{
private:
    /* data */
public:
    int i = 0;

    void add()
    {
        pthread_spin_lock(&s);
        i++;
        pthread_spin_unlock(&s);
    }
};
std::vector<Item> list1(MAX);
std::vector<Item> list2(MAX);

static void *threadFunc(void *arg)
{
    long v = reinterpret_cast<long>(arg);
    for (size_t i = v; i < MAX; i = i + 2)
    {
        list1[i].add();
    }
}
static void *threadFunc2(void *arg)
{
    long v = reinterpret_cast<long>(arg);
    for (size_t i = v; i < MAX; i = i + 2)
    {
        list1[i].add();
    }
}

int main(int argc, char const *argv[])
{
    pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);

    time_t time1, time2;
    time(&time1);
    std::cout << "++++" << std::endl;

    pthread_t t1, t2;
    int res;
    res = pthread_create(&t1, NULL, threadFunc, (void *)1);

    res = pthread_create(&t2, NULL, threadFunc2, (void *)2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    time(&time2);

    std::cout << "-----" << time2 - time1 << std::endl;
    return 0;
}
