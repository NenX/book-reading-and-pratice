#include "vector.h"

int multcnt = 0;

void multvec(int *x, int *y, int *z, int n)
{
    int i = 0;
    multcnt++;
    for (; i < n; i++)
    {
        z[i] = x[i] * y[i];
    }
}