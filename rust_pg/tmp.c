#include "stdio.h"
#define scrBegin         static int scrLine = 0; switch(scrLine) { case 0:;
#define scrFinish(z)     } return (z)
#define scrFinishV       } return

#define scrReturn(z)     \
        do {\
            scrLine=__LINE__;\
            return (z); case __LINE__:;\
        } while (0)
#define scrReturnV       \
        do {\
            scrLine=__LINE__;\
            return; case __LINE__:;\
        } while (0)


int function(void) {
    static int i;
    scrBegin;
    for (i = 0; i < 10; i++)
        scrReturn(i);
    scrFinish(0);
}

int main(int argc, char const *argv[])
{
    int i1 = function();
    int i2 = function();
    printf("%d %d",i1,i2);
    return 0;
}
