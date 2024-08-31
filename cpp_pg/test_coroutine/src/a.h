// coro_task.h
#ifndef COROUTINE_CORO_TASK_H
#define COROUTINE_CORO_TASK_H

#include <coroutine>
#include <exception>
#include <type_traits>

int test_co();

class Task
{
public:
    struct promise_type;
    using TaskHd1 = std::coroutine_handle<promise_type>;

private:
    TaskHd1 hd1_;

public:
    Task(auto h) : hd1_{h}
    {
        std::cout << "construction ~~~\n";
    }
    ~Task()
    {
        if (hd1_)
        {
            std::cout << "destroy ~~~\n";

            hd1_.destroy();
        }
    }

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    bool resume()
    {
        if (!hd1_ || hd1_.done())
            return false;
        hd1_.resume();
        return true;
    }

public:
    struct promise_type
    {
        /* data */
        auto get_return_object()
        {
            std::cout << "get_return_object ~~~\n";

            return Task{TaskHd1::from_promise(*this)};
        }
        auto initial_suspend()
        {
            std::cout << "initial_suspend ~~~\n";

            // return std::suspend_never{};
            return std::suspend_always{};
        }
        void unhandled_exception()
        {
            std::cout << "unhandled_exception ~~~\n";

            std::terminate();
        }
        void return_void()
        {
            std::cout << "return_void ~~~\n";
        }
        auto final_suspend() noexcept
        {
            std::cout << "final_suspend ~~~\n";

            return std::suspend_always{};
        }
    };
};
#endif // COROUTINE_CORO_TASK_H