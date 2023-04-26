#include <iostream>

#include <iostream>

struct foo
{
    int b ;
    int a ;
};

struct bar
{
    bar();
    int b = 222;
    foo f = {333};
};

bar::bar() = default;

void myfunc()
{
    int s;
    bar b{};

    std::cout << "bb:" << b.f.a << std::endl;
}
