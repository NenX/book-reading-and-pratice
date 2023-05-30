#include "vector.h"
#include <unistd.h>
#include <stdio.h>
void _addvecImpl(int *x, int *y, int *z, int n)
{
    int i = 0;
    for (; i < n; i++)
    {
        z[i] = x[i] + y[i];
    }
    printf("in _addvecImpl\n");
    sleep(-1);
}