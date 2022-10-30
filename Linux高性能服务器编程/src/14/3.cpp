// 多线程程序中调用fork

#include "pthread.h"
#include "iostream"
#include "unistd.h"
#include "wait.h"
using std::cout;
using std::endl;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void prepare()
{
    pthread_mutex_lock(&mutex);
}

void infork()
{
    pthread_mutex_unlock(&mutex);
}

void *another(void *arg)
{

    cout << "I'm child thread, lock the mutex" << endl;
    pthread_mutex_lock(&mutex);
    sleep(8);
    pthread_mutex_unlock(&mutex);
    cout << "I'm child thread, release  the mutex" << endl;
}

int main(int argc, char const *argv[])
{
    pthread_t p;

    pthread_create(&p, NULL, another, NULL);

    // fock 之前
    pthread_atfork(prepare, infork, infork);

    sleep(1);

    int pid = fork();

    if (pid == -1)
    {
        pthread_join(p, NULL);
        pthread_mutex_destroy(&mutex);
        return 1;
    }
    else if (pid == 0)
    {
        cout << "I'm the child pross, want to get the lock" << endl;

        pthread_mutex_lock(&mutex);
        cout << "I get the lock" << endl;
        pthread_mutex_unlock(&mutex);

        exit(0);
    }
    else
    {
        wait(NULL);
        pthread_mutex_destroy(&mutex);
        pthread_join(p, NULL);

        return 0;
    }

    return 0;
}
