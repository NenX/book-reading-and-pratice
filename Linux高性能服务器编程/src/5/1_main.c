#include <stdio.h>

void byteorder()
{
    long size_of_short = sizeof(short);
    union
    {
        short value;
        char union_bytes[size_of_short];
    } test;

    test.value = 0x0102;

    if ((test.union_bytes[0] == 1) && (test.union_bytes[size_of_short - 1] == 2))
    {
        printf("big endian!\n");
    }
    else if ((test.union_bytes[size_of_short - 1] == 1) && (test.union_bytes[0] == 2))
    {
        printf("little endian!\n");
    }
    else
    {
        printf("unkown!\n");
    }
}

void main()
{
    byteorder();
}