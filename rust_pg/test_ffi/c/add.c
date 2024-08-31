#include <stdio.h>
#include <stdint.h>

int32_t add(int32_t a, int32_t b)
{
    printf("source add [C] Hello from C !\n");
    printf("source add [C] Input a is: %i \n", a);
    printf("source add [C] Input b is: %i \n", b);
    printf("source add [C] Multiplying and returning result to Rust..\n");

    return a + b;
}
