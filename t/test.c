#include <stdio.h>

void test()
{
    int a[] = {2, 4, 9, 6};
    int *prt = (int *)(&a + 1);
    int b = *(a + 1);
    int c = *(prt - 2);

    printf("b ===== %d\n", b);
    printf("c ===== %d\n", c);
}

void main()
{
    test();
    printf("test");
}