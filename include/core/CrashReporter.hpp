#pragma once

#include "core/Logger.hpp"

#include <csignal>
#include <exception>
#include <iostream>

#include <cpptrace/cpptrace.hpp>

namespace mc::core
{

class CrashReporter
{
public:
    static void init()
    {
        std::set_terminate([]() {
            LOG(CRITICAL, "Unhandled exception! Dumping stack trace...");
            logStacktrace();
            std::abort();
        });

        std::signal(SIGSEGV, handleSignal);
        std::signal(SIGABRT, handleSignal);
        std::signal(SIGILL, handleSignal);
        std::signal(SIGFPE, handleSignal);

        LOG(INFO, "CrashReporter initialized");
    }

private:
    static void handleSignal(int)
    {
        LOG(CRITICAL, "Caught fatal signal {}. Dumping stack trace...",);
        logStacktrace();
        std::abort();
    }

    static void logStacktrace()
    {
        const auto trace = cpptrace::generate_trace();
        std::ostringstream oss;
        trace.print(oss);
        LOG(CRITICAL, "\n============== Stack trace ==============\n{}", oss.str());
    }
};

} // namespace mc::core
