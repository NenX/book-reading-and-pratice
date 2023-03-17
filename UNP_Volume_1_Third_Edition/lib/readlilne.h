#include "unp.h"


#ifndef __unp_readline_h
#define __unp_readline_h

static int read_cnt;
static char * read_ptr;
static char read_buf[MAXLINE]; 

ssize_t readline_slow(int fd, void* vptr, size_t maxlen);
ssize_t readline(int fd, void* vptr, size_t maxlen);
ssize_t readlinebuf(void **vptrptr);
#endif 

