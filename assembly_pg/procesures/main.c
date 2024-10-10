
long remote(long i);
long call_and_ret(int *a, int *b, int *c, int *d, int *e, long *f, long s1, int s2, short s3, char s4)
{
    return *a + *b + *c + *d + *e + *f + s1 + s2 + s3 + s4;
}

void main()
{

    int arr[5] = {0, 1, 2, 3, 4};
    long p2 = 99;
    int p1 = 88;
    long result = call_and_ret(arr + 1, arr + 2, arr + 3, arr + 4, &p1, &p2, 11, 22, 33, 44);
    remote(result);
}