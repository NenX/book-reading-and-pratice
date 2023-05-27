
// 
// syscall 调用见 #include <sys/syscall.h>
// 编译 gcc -c -fno-stack-protector -fno-builtin nomain.cpp
// 链接
// 第一种：ld -e nomain nomain.o
// 第二种：ld -T nomain.lds nomain.o
asm(
    ".text\n"
    ".global Add\n"
    ".type Add, @function\n"
    "Add:\n"
    "leal 1(%rdi,%rsi), %eax\n"
    "ret");
char str[] = {'a', 'b', 'c', '?', '\n', 0};

extern "C" void pp(char c)
{
    str[3] = c;
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
    pp('a' + a);
    myexit();
    return 0;
}