#pragma once

#include "global_vars.hpp"

#include <chrono>
#include <concurrencpp/concurrencpp.h>

using namespace std::chrono_literals;

template <class PREDICATE>
[[nodiscard]] concurrencpp::result<bool> wait_until(
    PREDICATE pred,
    std::chrono::milliseconds interval = 100ms,
    std::chrono::milliseconds timeout = 5s)
{
    const auto start = std::chrono::steady_clock::now();

    while (true)
    {
        if (pred())
        {
            co_return true;
        }

        const auto now = std::chrono::steady_clock::now();
        if (now - start >= timeout)
        {
            co_return false;
        }

        co_await g_runtime.timer_queue()->make_delay_object(interval, g_runtime.thread_pool_executor());
    }
}
