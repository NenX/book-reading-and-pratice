// Thread-Safe Bounded Queue
#include <iostream>
#include <list>
using namespace std;
int main(int argc, char const *argv[])
{
    list<int> l;
    l.push_back(0);
    l.push_back(1);
    std::cout << "playground:" << l.front() << std::endl;
    l.pop_front();
    std::cout << "playground:" << l.front() << std::endl;
    l.pop_front();

    return 0;
}
