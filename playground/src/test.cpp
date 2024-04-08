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
    myUnion u;
    u.s = 0x0102;
    size_t fd = open("test", O_CREAT | O_RDWR);
    write(fd, &u, sizeof(u));
    std::cout << 1 << std::endl;
    std::cout << (u.c[0] == 1) << std::endl;
    std::cout << (u.c[1] == 1) << std::endl;
    return 0;
}
