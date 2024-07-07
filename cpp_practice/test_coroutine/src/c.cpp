#include <concepts>
#include <coroutine>
#include <exception>
#include <iostream>
#include "c.h"
struct ReturnObject
{
public:
    struct promise_type
    {
        unsigned value_;
        ReturnObject get_return_object()
        {
            return {.h = h_type::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        std::suspend_always yield_value(unsigned v)
        {
            value_ = v;
            return {};
        }
    };
    operator std::coroutine_handle<promise_type>() const { return h; }
    operator std::coroutine_handle<>() const { return h; }

    using h_type = std::coroutine_handle<promise_type>;

    h_type h;
};

template <typename P>
struct Get_Promise
{
    P *promise;
    std::coroutine_handle<> *hp_;
    constexpr bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<P> h)
    {
        promise = &h.promise();
        return false;
    }
    constexpr P *await_resume() const noexcept
    {
        return promise;
    }
};

ReturnObject
couter_c()
{
    auto p = co_await Get_Promise<ReturnObject::promise_type>{};
    for (unsigned i = 0;; ++i)
    {
        // p->value_ = i;
        // co_await std::suspend_always{};
        co_yield (i + 100);
    }
}

int test_c()
{

    ReturnObject::h_type h = couter_c();
    ReturnObject::promise_type *p = &h.promise();
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "In main1 " << p->value_ << std::endl;
        h.resume();
    }
    h.destroy();
    return 0;
}