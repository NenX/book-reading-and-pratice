#include <pthread.h>
#include "tlpi_hdr.h"
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t con;

pthread_cond_t empty_con;

static volatile long count;
static void *
threadFunc(void *arg)
{
    long n = (long)arg;
    while (1)
    {

        pthread_mutex_lock(&mutex);

        if (count > 100000)
        {
            printf("peer thread %ld: %ld\n", n, count);
            pthread_cond_signal(&con);
            pthread_cond_wait(&empty_con, &mutex);
        }
        count++;

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
int main(int argc, char *argv[])
{
    pthread_t t1;
    pthread_t t2;

    int s;
    s = pthread_create(&t1, NULL, threadFunc, (void *)1);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, (void *)2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    if (s != 0)
        errExitEN(s, "pthread_join");

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (count <= 0)
        {
            pthread_cond_wait(&con, &mutex);
        }
        count = 0;
        printf("main thread consumes %ld \n", count);
        sleep(1);
        pthread_cond_signal(&empty_con);
        pthread_mutex_unlock(&mutex);
    }
}