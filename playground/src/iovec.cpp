#include <iostream>
#include <sys/uio.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct Aa
{
    unsigned int a1;
    char b1;
    char b2;
    char b3;
    char b4;
};

int main()
{
    iovec arr[3];

    unsigned int i = 0x41424344;
    arr[0].iov_base = &i;
    arr[0].iov_len = sizeof(unsigned int);

    Aa a{0x64646261, '1', '2', '3', '4'};
    arr[1].iov_base = &a;
    arr[1].iov_len = sizeof(a);

    char *str = "fuck";
    arr[2].iov_base = str;
    arr[2].iov_len = strlen(str);

    int fd = open("./abc.txt", O_CREAT | O_RDWR);

    int res = writev(fd, arr, 3);

    std::cout << "test vecio" << std::endl;
}