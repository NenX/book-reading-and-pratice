#include <stdio.h>
#include <stdint.h>

int32_t static_call(int32_t a, int32_t b) {
    printf("static_call add [C] Hello from C !\n");
    printf("static_call add [C] Input a is: %i \n", a);
    printf("static_call add [C] Input b is: %i \n", b);
    printf("static_call add [C] Multiplying and returning result to Rust..\n");

    return a + b;
}
