#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
union myUnion
{
    short s;
    char c[sizeof(short)];
};

int main(int argc, char const *argv[])
{
    int i = 22;
    int i2 = 33;
    int * p = &i;
    int **pp = &p;
    *pp = &i2;
    std::cout << *p << std::endl;
    return 0;
}
