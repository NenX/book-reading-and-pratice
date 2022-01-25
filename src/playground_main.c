#include <pthread.h>
#include "tlpi_hdr.h"
#include <unistd.h>
#include <fcntl.h>
#define MAX_NUM 9999
char buf[MAX_NUM];
int main(int argc, char *argv[])
{
        printf("main start\n");

    size_t result;
    int fd;
    fd = open("../.gitignore", O_RDONLY);
    if (fd == -1)
    {
        errExit("open");
    }
    while ((result = read(STDIN_FILENO, buf, MAX_NUM)) >= 0)
    {
        buf[result] = '\0';
        printf("num:%ld, data: %s", result, buf);

        if(result==0){
            pthread_exit(NULL);
        }
    }


}