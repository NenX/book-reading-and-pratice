#include <stdlib.h>
#include "tlpi_hdr.h"

extern char** environ;

int main(int argc,char * argv[]){
    clearenv();
    
    for (size_t i = 1; i < argc; i++)
    {
        putenv(argv[i]);
    }
    unsetenv("SB");
    setenv("BYE","BYE",0);

    char ** ep = environ;

    while (ep != NULL && *ep != NULL)
    {
        printf("env: %s\n",*ep);
        ep++;
    }
    

    exit(EXIT_SUCCESS);
}