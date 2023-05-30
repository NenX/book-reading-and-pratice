#include "vector.h"

int addcnt = 0;

void addvec(int *x, int *y, int *z, int n)
{
    int i = 0;
    addcnt++;
    for (; i < n; i++)
    {
        z[i] = x[i] + y[i];
    }
}