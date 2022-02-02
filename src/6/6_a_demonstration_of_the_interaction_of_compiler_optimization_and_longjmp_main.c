#include "tlpi_hdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env;

void doJmp(int nvar, int rvar, int vvar)
{
    printf("Inside doJump(): nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);
    longjmp(env, 1);
}

int main(int argc, char *argv[])
{
    int nvar;
    register int rvar;
    volatile int vvar;
    nvar = 111;
    rvar = 222;
    vvar = 333;
    if (setjmp(env) == 0)
    {
        nvar = 444;
        rvar = 555;
        vvar = 666;
        doJmp(nvar, rvar, vvar);
    }
    else
    {
        printf("After longjmp(): nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);
    }

    exit(EXIT_SUCCESS);
}