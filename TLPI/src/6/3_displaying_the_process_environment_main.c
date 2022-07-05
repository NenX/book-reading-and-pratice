#include "tlpi_hdr.h"

extern char **environ;
void main()
{
    char **ep = environ;
    while (*ep != NULL)
    {
        puts(*ep);
        ep++;
    }
    
}