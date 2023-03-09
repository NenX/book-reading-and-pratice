#include <cstdio>
#include <iostream>
#include <functional>
int main()
{
    int x = 2;
    auto f = [=](int a, int b) mutable
    { return (x = x + a + b*2); };
    auto ff = std::bind(f, std::placeholders::_1, 2);
    std::cout << ff(2) << std::endl;
    std::cout << ff(2) << std::endl;
    std::cout << ff(2) << std::endl;
    std::cout << x << std::endl;
}