#include <stdio.h> 
 
asm( 
    ".text\n" 
    ".global Add\n" 
    ".type Add, @function\n" 
    "Add:\n" 
    "leal 2(%rdi,%rsi,8), %eax\n" 
    "ret" 
); 

extern int Add(int, int); 
int main(int argc,char** argv) 
{ 
    int b=1,c=2; 
    int a = Add(b,c); 
    printf("res =  %d\n", a); 
    return 0; 
} 