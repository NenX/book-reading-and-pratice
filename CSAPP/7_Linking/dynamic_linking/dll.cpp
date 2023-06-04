#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#define MAX_N 2

valuetype x[MAX_N] = {1, 2};
valuetype y[MAX_N] = {4, 5};
valuetype z[MAX_N] = {0x99, 0x88};
void *handle_libvector;
void *handle_libvectorimpl;
char *error;
typedef void (*my_addvec)(valuetype *x, valuetype *y, valuetype *z, valuetype n);
my_addvec _addvec;
int main(int argc, char const *argv[])
{

    handle_libvector = dlopen("./libvector.so", RTLD_LAZY);
    if (!handle_libvector)
    {
        printf("dlopen libvector failed.\n");
        exit(1);
    }
    handle_libvectorimpl = dlopen("./libvectorimpl.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!handle_libvectorimpl)
    {
        printf("dlopen libvectorimpl failed.\n");
        exit(1);
    }
    _addvec = (my_addvec)(dlsym(handle_libvector, "addvec"));

    if ((error = dlerror()) != NULL)
    {
        printf("dlsym failed.\n");
        exit(1);
    }
    _addvec(x, y, z, MAX_N);
    long long z0 = z[0];
    long long z1 = z[1];
    printf("z = [%lld %lld]\n", z0, z1);

    if (dlclose(handle_libvector) < 0)
    {
        printf("dlclose failed.\n");
        exit(1);
    }
    return 0;
}
