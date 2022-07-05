#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "curr_time.h" /* Declaration of currTime() */
#include "semun.h"     /* Definition of semun union */
#include "tlpi_hdr.h"
int main(int argc, char *argv[])
{
    int semid, key, perms;
    struct sembuf * sops;

    /* Create a set containing 1 semaphore */
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | perms);
    if (semid != -1)
    { /* Successfully created the semaphore */
        union semun arg;
        /* XXXX */
        arg.val = 0; /* Initialize semaphore */
        if (semctl(semid, 0, SETVAL, arg) == -1)
            errExit("semctl");
    }
    else
    {                        /* We didn't create the semaphore */
        if (errno != EEXIST) /* Unexpected error from semget() */
            errExit("semget");
        semid = semget(key, 1, perms); /* Retrieve ID of existing set */
        if (semid == -1)
            errExit("semget");
    }
    /* Now perform some operation on the semaphore */
    sops[0].sem_op = 1;  /* Add 1... */
    sops[0].sem_num = 0; /* to semaphore 0 */
    sops[0].sem_flg = 0;
    if (semop(semid, sops, 1) == -1)
        errExit("semop");
}