#define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
#include <stdio.h>
#include <string.h>
#include "weird.h"             /* Get declaration of strerror() */
#define MAX_ERROR_LEN 256       /* Maximum length of string  returned by strerror() */
static char buf[MAX_ERROR_LEN]; /* Statically allocated return buffer */
char *
strerror(int err)
{
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL)
    {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    }
    else
    {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0'; /* Ensure null termination */
    }
    return buf;
}

void main()
{
    char *err = strerror(3);
    printf("error: %s\n", err);
}