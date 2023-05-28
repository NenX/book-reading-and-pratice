#include <stdio.h>
#include "Lib.h"
#include <unistd.h>
void foobar(int i)
{
    printf("Printing from Lib.so ??????? %d.\n", i);
    sleep(-1);
}