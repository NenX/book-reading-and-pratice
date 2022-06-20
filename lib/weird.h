#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
/* could not find sys_errlist.h  ?????*/
#if GCC_VERSION > 70500
int _sys_nerr;
const char *const _sys_errlist[];
#endif