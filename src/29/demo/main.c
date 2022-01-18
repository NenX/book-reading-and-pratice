#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
void *fn(void *data)
{
    pthread_t *d = (pthread_t *)data;
    printf("thread:%ld:%ld\n", pthread_self(), pthread_equal(*d, pthread_self()));

    return NULL;
}
int main()
{

    pthread_t thread;
    pthread_create(&thread, NULL, fn, (void *)&thread);

    pthread_join(thread, NULL);

    pthread_exit(NULL);

    return 0;
}