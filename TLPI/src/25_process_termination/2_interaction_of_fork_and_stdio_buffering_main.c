#include "tlpi_hdr.h"
#include "curr_time.h"

/*
    ./25_process_termination_2_interaction_of_fork_and_stdio_buffering_main > a.txt && cat ./a.txt
    what happens?
*/
int main(int argc, char *argv[])
{

    pid_t pid;
    printf("[%s %d] Hello world\n", currTime("%T"), getpid());

    /* one way */

    // fflush(stdout);

    write(STDOUT_FILENO, "Ciao\n", 5);
    if ((pid = fork()) == -1)
        errExit("fork");
    /* Both child and parent continue execution here */
    exit(EXIT_SUCCESS);

    /* another say */

    // if (pid == 0)
    // {
    //     _exit(EXIT_SUCCESS);
    // }
    // else
    // {
    //     exit(EXIT_SUCCESS);
    // }
}