#include "vector.h"

int addcnt = 0;

void addvec(valuetype *x, valuetype *y, valuetype *z, valuetype n)
{
    addcnt++;
    _addvecImpl(x, y, z, n);
}