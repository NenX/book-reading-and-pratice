#include <cstdio>
#include <iostream>
#include <functional>

union short_wrap
{
    short s;
    char c[sizeof(s)];
};

void check()
{
    short_wrap sw;
    sw.s = 0x0102;
    if (sizeof(short) == 2)
    {
        if (sw.c[0] == 1 && sw.c[1] == 2)
        {
            std::cout << "big endian" << std::endl;
        }
        else if (sw.c[0] == 2 && sw.c[1] == 1)
        {
            std::cout << "little endian" << std::endl;
        }
        else
        {
            std::cout << "unknown endian" << std::endl;

        }
    }
}

int main()
{
    int x = 2;
    auto f = [&](int a, int b) mutable
    { return (x = x + a + b * 2); };
    auto ff = std::bind(f, std::placeholders::_1, 2);
    std::cout << ff(2) << std::endl;
    std::cout << ff(2) << std::endl;
    std::cout << ff(2) << std::endl;
    std::cout << x << std::endl;
    check();
}