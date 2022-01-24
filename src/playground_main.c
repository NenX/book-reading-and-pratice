#include <pthread.h>
#include "tlpi_hdr.h"
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t con;

pthread_cond_t empty_con;
static volatile long count;
pthread_key_t t1;
pthread_key_t t2;
pthread_once_t once;

void destrctor(void *data)
{
    printf("destrctor : %d\n", (int)data);
}

void fn(void)
{
    printf("once !!\n");
    pthread_key_create(&t1, destrctor);
    pthread_key_create(&t2, destrctor);
}
static void *
threadFunc(void *arg)
{

    pthread_once(&once, fn);
    long n = (long)arg;

    if (n == 111)
    {
        pthread_setspecific(t2, (void *)777);
        void *data = pthread_getspecific(t2);
        printf("thread specific data %ld:%u:%ld\n", n, t2, (int)data);
    }
    else
    {
        void *data = pthread_getspecific(t2);
        printf("thread specific data %ld:%u:%ld\n", n, t2, (int)data);
    }

    printf("thread %ld:%u-%u\n", n, t1, t2);
    return NULL;
}

int main(int argc, char *argv[])
{
    printf("main 123\n");

    pthread_t t1;
    pthread_t t2;

    int s;
    s = pthread_create(&t1, NULL, threadFunc, (void *)111);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, (void *)222);
    if (s != 0)
        errExitEN(s, "pthread_create");

    if (s != 0)
        errExitEN(s, "pthread_join");
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}