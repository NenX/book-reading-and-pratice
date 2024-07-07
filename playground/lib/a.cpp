#include <iostream>
#include <vector>

#include <algorithm>
#include <set>
struct foo
{
    int b;
    int a;
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
    // std::set<int> is{9, 100, 8, 2, 7, 3, 6, 4};
    // auto i = std::back_inserter(is);
    // *i=99;
    // std::for_each(is.begin(),is.end(),[](auto &i){
    //     std::cout << i << std::endl;
    // });
}
