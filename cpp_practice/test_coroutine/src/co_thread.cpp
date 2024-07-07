
#include "co_thread.h"
auto switch_to_new_thread(std::jthread &out)
{
    struct awaitable
    {
        std::jthread *p_out;
        bool await_ready()
        {
            std::cout << "3 await_ready thread: " << std::this_thread::get_id() << std::endl;

            return false;
        }
        void await_suspend(std::coroutine_handle<> h)
        {

            std::jthread &out = *p_out;
            if (out.joinable())
                throw std::runtime_error("Output jthread parameter not empty");
            out = std::jthread([h]
                               { 
                                std::cout << "\na before sleep ------------------------------------------------------------>" << std::this_thread::get_id() << std::endl;
                                sleep(2);
                                std::cout << "\nb after sleep ------------------------------------------------------------>" << std::this_thread::get_id() << std::endl;

                                h.resume(); });
            // Potential undefined behavior: accessing potentially destroyed *this
            // std::cout << "New thread ID: " << p_out->get_id() << '\n';
            std::cout << "4 New thread:  " << out.get_id() << " parent thread: " << std::this_thread::get_id() << '\n'; // this is OK
        }
        void await_resume()
        {
            std::cout << "\nc await_resume ------------------------------------------------------------>" << std::this_thread::get_id() << std::endl;
        }
    };
    return awaitable{&out};
}

struct task
{
    struct promise_type
    {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};

task resuming_on_new_thread(std::jthread &out)
{
    std::cout << "2 Coroutine started on thread: " << std::this_thread::get_id() << '\n';
    co_await switch_to_new_thread(out);
    // awaiter destroyed here
    std::cout << "\nd Coroutine resumed on thread ------------------------------" << std::this_thread::get_id() << '\n';
}

int test_co_thread()
{
    std::cout << "1 main first line, thread: " << std::this_thread::get_id() << std::endl;

    std::jthread out;
    resuming_on_new_thread(out);
    std::cout << "5 main last line, thread: " << std::this_thread::get_id() << std::endl;
}