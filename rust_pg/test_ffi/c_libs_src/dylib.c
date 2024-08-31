#include <stdio.h>
#include <stdint.h>
#ifdef _WIN32
__declspec(dllexport)
#endif
int32_t
dylib_call(int32_t a, int32_t b)
{
    printf("dylib_call add [C] Hello from C !\n");
    printf("dylib_call add [C] Input a is: %i \n", a);
    printf("dylib_call add [C] Input b is: %i \n", b);
    printf("dylib_call add [C] Multiplying and returning result to Rust..\n");

    return a * b;
}
