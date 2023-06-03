#include "vector.h"
#include <stdio.h>
#define MAX_N 2

valuetype x[MAX_N] = {1, 2};
valuetype y[MAX_N] = {4, 5};
valuetype z[MAX_N] = {0x99,0x88};

int main(int argc, char const *argv[])
{
    addvec(x, y, z, MAX_N);
    long long z0 = z[0];
    long long z1 = z[1];
    printf("z = [%lld %lld]\n", z0, z1);
    return 0;
}
