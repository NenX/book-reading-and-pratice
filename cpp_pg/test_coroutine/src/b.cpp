#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>

struct ReturnObject
{
public:
    struct promise_type
    {
        ReturnObject get_return_object()
        {
            return {.h = h_type::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
    };
    operator std::coroutine_handle<promise_type>() const { return h; }
    operator std::coroutine_handle<>() const { return h; }

    using h_type = std::coroutine_handle<promise_type>;

    h_type h;
};

struct Awaiter
{

    std::coroutine_handle<> *hp_;
    mutable int num = 0;
    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {}
    constexpr int await_resume() const noexcept
    {
        num = num + 10;
        return ++num;
    }
};

ReturnObject
counter()
{
    Awaiter a{};
    int n;
    for (unsigned i = 0;; ++i)
    {
        n = co_await a;
        std::cout << "counter: " << i << " num: " << n << std::endl;
    }
}

int test_b()
{

    ReturnObject::h_type h = counter();
    auto i = h.promise();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "In main1 function\n";
        h.resume();
    }
    h.destroy();
    return 0;
}