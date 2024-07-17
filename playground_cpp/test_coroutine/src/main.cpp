// 需要 c++20，cmake高版本才支持
// 教程 https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
#include <iostream>
#include <set>
#include <map>
#include <tuple>
#include <string>
#include <functional>
#include "co_thread.h"
#include "d.h"


int main()
{
    test_co_thread();
    test_d();
    std::cout << "eol thread: " << std::this_thread::get_id() << std::endl;
    return 0;
}