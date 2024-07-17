// main.cpp
#include <iostream>
#include "a.h"
#include <set>
#include <map>
#include <tuple>
#include <string>
#include <unistd.h>
#include <iostream>
Task qqq(int max)
{
    sleep(2);

    std::cout << "qqq hello world\n";
    for (int i = 0; i < max; ++i)
    {
        std::cout << "hello " << i << "\n";
        sleep(2);
        co_await std::suspend_always{};
    }

    std::cout << "qqq hello end\n";
}

int test_co()
{
    std::cout << "test_co start ~~~\n";

    auto co = qqq(3);

    while (co.resume())
    {
        std::cout << "hello coroutine suspend !!!!!!!!!!!\n";
    }
    std::cout << "test_co end ~~~\n";

    return 0;
}