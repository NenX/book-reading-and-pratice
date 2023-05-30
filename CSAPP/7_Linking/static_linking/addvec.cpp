#include "vector.h"

int addcnt = 0;

void addvec(int *x, int *y, int *z, int n)
{
    addcnt++;
    _addvecImp(x, y, z, n);
}