#include "vector.h"

void _addvecImp(int *x, int *y, int *z, int n)
{
    int i = 0;
    for (; i < n; i++)
    {
        z[i] = x[i] + y[i];
    }
}