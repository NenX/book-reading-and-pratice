#include <pthread.h>
#include "tlpi_hdr.h"
#include <unistd.h>
#include <fcntl.h>
#include "curr_time.h"
#define MAX_NUM 9999
char buf[MAX_NUM];
int main(int argc, char *argv[])
{
    printf("aaa");
    printf("bbb");
    write(STDOUT_FILENO,"111",3);

    sleep(3);
    printf("ccc");

}