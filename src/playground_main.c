#include <pthread.h>
#include "tlpi_hdr.h"
#include <unistd.h>
static void *
threadFunc(void *arg)
{
    char *str = (char *)arg;
    for (size_t i = 0; i < 100; i++)
    {
        sleep(1);
        printf("thread %s: %ld \n", str, i);
    }

    return NULL;
}
int main(int argc, char *argv[])
{
    pthread_t t1;
    int s;
    s = pthread_create(&t1, NULL, threadFunc, "Hello world");
    if (s != 0)
        errExitEN(s, "pthread_create");
    printf("Message from main()\n");

    // s = pthread_join(t1, NULL);

    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("Thread returned \n");
    pthread_exit(EXIT_SUCCESS);
}