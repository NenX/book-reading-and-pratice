#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int count;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t threadDeadCond = PTHREAD_COND_INITIALIZER;
enum E
{
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED
};
struct item
{
    pthread_t pid;
    enum E status;
    int sleepTime;
    int threadNum;
};
void *thread1(void *p)
{

    struct item *v = (struct item *)p;
    sleep(v->sleepTime);

    pthread_mutex_lock(&lock);
    count++;
    printf("item:%d,%d\n", count, v->sleepTime);
    if (count == v->threadNum)
    {
        pthread_cond_signal(&threadDeadCond);
    }

    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(int argc, char const *argv[])
{

    int num;
    struct item *itemPtr;

    itemPtr = calloc(argc - 1, sizeof(*itemPtr));
    for (size_t i = 0; i < argc - 1; i++)
    {
        itemPtr[i].status = TS_ALIVE;
        itemPtr[i].threadNum = argc - 1;
        itemPtr[i].sleepTime = atoi(argv[i + 1]);
        pthread_create(&itemPtr[i].pid, NULL, thread1, &itemPtr[i]);
    }

    pthread_mutex_lock(&lock);
    while (count != argc - 1)
    {
        pthread_cond_wait(&threadDeadCond, &lock);
    }

    pthread_mutex_unlock(&lock);

    return 0;
}
