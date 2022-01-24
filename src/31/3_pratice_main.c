#define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from <stdio.h> */
#include <errno.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_ERROR_LEN 256       /* Maximum length of string  returned by strerror() */

static pthread_once_t once;
static pthread_key_t key;

void destr(void *buf)
{
    free(buf);
}
void init(void)
{
    pthread_key_create(&key, destr);
}

char *
strerror(int err)
{
    char * buf;
    pthread_once(&once, init);
    buf = pthread_getspecific(key);
    if(buf == NULL){
        buf = malloc(MAX_ERROR_LEN);
        pthread_setspecific(key,buf);
    }
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

#include <pthread.h>
#include "tlpi_hdr.h"
static void *
threadFunc(void *arg)
{
    char *str;
    printf("Other thread about to call strerror()\n");
    str = strerror(EPERM);
    printf("Other thread: str (%p) = %s\n", str, str);
    return NULL;
}
int main(int argc, char *argv[])
{
    pthread_t t;
    int s;
    char *str;
    str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");
    s = pthread_create(&t, NULL, threadFunc, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_join(t, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    printf("Main thread: str (%p) = %s\n", str, str);
    exit(EXIT_SUCCESS);
}