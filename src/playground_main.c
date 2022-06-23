#define _XOPEN_SOURCE

#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include "curr_time.h" /* Declaration of currTime() */
#include "semun.h"     /* Definition of semun union */
#define MAX_NUM 9999

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        usageErr("错误的使用\n");
    }
    if (argc == 2)
    {
        int semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (semid == -1)
            errExit("semid");
        union semun args;
        args.val = getInt(argv[1], 0, "init-value");
        if (semctl(semid, 0, SETVAL, &args) == -1)
            errExit("semctl");
        printf("semaphore id: %d\n", semid);
    }
    else
    {
        int semid = getInt(argv[1], 0, "semid");
        struct sembuf sop;
        sop.sem_num = 0;
        sop.sem_op = getInt(argv[2], 0, "operation");
        sop.sem_flg = 0;

        printf("%ld: about to semop at %s", (long)getpid(), currTime("%T"));
        if (semop(semid, &sop, 1))
            printf("%ld: semop completed at %s", (long)getpid(), currTime("%T"));
    }
}