#include "unp.h"



#ifndef __unp_readline_h
#define __unp_readline_h
ssize_t readline_slow(int fd, void* vptr, size_t maxlen);
ssize_t readline(int fd, void* vptr, size_t maxlen);
ssize_t readlinebuf(void **vptrptr);
#endif 

