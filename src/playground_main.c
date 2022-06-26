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
#include "./47_System_V_Semaphores/semun.h"     /* Definition of semun union */
#define MAX_NUM 9999

int main(int argc, char *argv[])
{
    // int semid;
    // if (argc < 2 || argc > 3)
    // {
    //     usageErr("错误的使用\n");
    // }
    // if (argc == 2)
    // {
    //     union semun arg;
    //     semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    //     if (semid == -1)
    //         errExit("semid");
    //     arg.val = getInt(argv[1], 0, "init-value");
    //     if (semctl(semid, 0, SETVAL, arg) == -1)
    //         errExit("semctl");
    //     printf("semaphore id: %d\n", semid);
    // }
    // else
    // {
    //     struct sembuf sop;
    //     semid = getInt(argv[1], 0, "semid");
    //     sop.sem_num = 0;
    //     sop.sem_op = getInt(argv[2], 0, "operation");
    //     sop.sem_flg = 0;

    //     printf("%ld: about to semop at %s", (long)getpid(), currTime("%T"));
    //     if (semop(semid, &sop, 1) == -1)
    //         errExit("semop");
    //     printf("%ld: semop completed at %s", (long)getpid(), currTime("%T"));
    // }

    union semun s;
    s.val = 222;
    printf("??:%d",s.val);

}