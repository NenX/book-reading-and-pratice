#include "vector.h"

int multcnt = 0;

void multvec(valuetype *x, valuetype *y, valuetype *z, valuetype n)
{
    int i = 0;
    multcnt++;
    for (; i < n; i++)
    {
        z[i] = x[i] * y[i];
    }
}