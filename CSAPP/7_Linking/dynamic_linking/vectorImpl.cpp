#include "vector.h"
#include <unistd.h>
#include <stdio.h>
void _addvecImpl(valuetype *x, valuetype *y, valuetype *z, valuetype n)
{
    valuetype i = 0;
    for (; i < n; i++)
    {
        z[i] = x[i] + y[i];
    }
    printf("in _addvecImpl\n");
    sleep(-1);
}