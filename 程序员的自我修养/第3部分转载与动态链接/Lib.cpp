#include <stdio.h>
#include "Lib.h"
#include <unistd.h>
const char * str = "Printing from Lib.so ??????? %d.\n";
void foobar(int i)
{
    printf(str, i);
    sleep(-1);
}