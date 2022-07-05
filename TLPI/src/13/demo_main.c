#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    printf("aaa");
    printf("bbb");
    fflush(stdout);
    printf("ccc");

    write(STDOUT_FILENO,"111",3);

    sleep(3);
    printf("ddd");

}