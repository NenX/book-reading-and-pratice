

#include <signal.h>
#include "tlpi_hdr.h"
int main(int argc, char const *argv[])
{
    sigset_t blockSet, prevMask;
    /* Initialize a signal set to contain SIGINT */
    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGINT);
    /* Block SIGINT, save previous signal mask */
    if (sigprocmask(SIG_BLOCK, &blockSet, &prevMask) == -1)
        errExit("sigprocmask1");
    /* ... Code that should not be interrupted by SIGINT ... */
    /* Restore previous signal mask, unblocking SIGINT */
    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask2");
    return 0;
}
