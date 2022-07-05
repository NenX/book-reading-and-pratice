#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "curr_time.h" /* Declaration of currTime() */
#include "semun.h"     /* Definition of semun union */
#include "tlpi_hdr.h"
int main(int argc, char *argv[])
{

    int semid,key,perms;
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | perms);
    if (semid != -1)
    { /* Successfully created the semaphore */
        union semun arg;
        struct sembuf sop;
        arg.val = 0; /* So initialize it to 0 */
        if (semctl(semid, 0, SETVAL, arg) == -1)
            errExit("semctl");
        /* Perform a "no-op" semaphore operation - changes sem_otime
        so other processes can see we've initialized the set. */
        sop.sem_num = 0; /* Operate on semaphore 0 */
        sop.sem_op = 0;  /* Wait for value to equal 0 */
        sop.sem_flg = 0;
        if (semop(semid, &sop, 1) == -1)
            errExit("semop");
    }
    else
    { /* We didn't create the semaphore set */
        const int MAX_TRIES = 10;
        int j;
        union semun arg;
        struct semid_ds ds;
        if (errno != EEXIST)
        { /* Unexpected error from semget() */
            errExit("semget");
            semid = semget(key, 1, perms); /* Retrieve ID of existing set */
            if (semid == -1)
                errExit("semget");
            /* Wait until another process has called semop() */
            arg.buf = &ds;
            for (j = 0; j < MAX_TRIES; j++)
            {
                if (semctl(semid, 0, IPC_STAT, arg) == -1)
                    errExit("semctl");
                if (ds.sem_otime != 0) /* semop() performed? */
                    break;             /* Yes, quit loop */
                sleep(1);              /* If not, wait and retry */
            }
            if (ds.sem_otime == 0) /* Loop ran to completion! */
                fatal("Existing semaphore not initialized");
        }
    }
}