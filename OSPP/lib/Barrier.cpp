#include "Barrier.h"

Barrier::Barrier(int n) : numThreads(n), numEntered(0)
{
    int res;
    res = pthread_mutex_init(&mtx, NULL);
    if (res != 0)
        std::cout << "Barrier mutex init failed!" << std::endl;
    res = pthread_cond_init(&allCheckedInCond, NULL);
    if (res != 0)
        std::cout << "Barrier itemAddedCond init failed!" << std::endl;
}

Barrier::~Barrier()
{
    int res;
    res = pthread_mutex_destroy(&mtx);
    if (res != 0)
        std::cout << "Barrier mutex destroy failed!" << std::endl;
    res = pthread_cond_destroy(&allCheckedInCond);
    if (res != 0)
        std::cout << "Barrier itemAddedCond destroy failed!" << std::endl;
}

void Barrier::checkin()
{
    pthread_mutex_lock(&mtx);
    numEntered++;
    if (numEntered < numThreads)
    {
        while (numEntered < numThreads)
        {
            pthread_cond_wait(&allCheckedInCond, &mtx);
        }
    }
    else
    {
        pthread_cond_broadcast(&allCheckedInCond);
        numLeaving = 0;
    }

    numLeaving++;

    if (numLeaving < numThreads)
    {
        while (numLeaving < numThreads)
        {
            pthread_cond_wait(&allLeavingCond, &mtx);
        }
    }
    else
    {
        pthread_cond_broadcast(&allLeavingCond);
        numEntered = 0;
    }
    pthread_mutex_unlock(&mtx);
}
