
//
// syscall 调用见 #include <sys/syscall.h>
// 编译 gcc -c -fno-stack-protector -fno-builtin nomain.cpp
// 链接
// 第一种：ld -e nomain nomain.o
// 第二种：ld -T nomain.lds nomain.o
#include "nomain.h"
asm(
    ".text\n"
    ".global Add\n"
    ".type Add, @function\n"
    "Add:\n"
    "leal 1(%rdi,%rsi), %eax\n"
    "ret");
char str[16] = {'a', 'b', 'c', '?', '\n', 0};
char str2[16] ;
extern char str3 ;
long long_value;
extern "C" void pp(char c)
{
    str[0] = c;
    str2[0] = c;
    str3 = c;
    long_value++;
    long long n;
    asm volatile(
        "syscall\t\n"
        : "=a"(n)
        : "a"(1), "S"(str), "D"(1), "d"(6));
}
void myexit()
{
    long long n;
    asm volatile(
        "syscall\t\n"
        : "=a"(n)
        : "a"(60));
}
extern "C" int Add(int, int);
extern "C" int nomain(int argc, char **argv)
{
    int b = 1, c = 2;
    int a = Add(b, c);
    char d = 'b' + a;
    pp(d + a_int);
    pp(d);
    myexit();
    return 0;
}