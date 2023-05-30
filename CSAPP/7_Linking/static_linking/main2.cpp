#include "vector.h"
#include <stdio.h>
#define MAX_N 2

int x[MAX_N] = {1, 2};
int y[MAX_N] = {4, 5};
int z[MAX_N] = {22,33};

int main(int argc, char const *argv[])
{
    addvec(x, y, z, MAX_N);
    printf("z = [%d %d]\n", z[0], z[1]);
    return 0;
}
