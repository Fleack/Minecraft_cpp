#pragma once

#include <format>
#include <memory>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>

#define LOG(level, fmt, ...)                                     \
    mc::core::Logger::get()->log(                                \
        spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
        to_spdlog_level(LogLevel::level),                        \
        std::format(fmt __VA_OPT__(, ) __VA_ARGS__))

// Uncomment to enable logs that might be spammy
#define ENABLE_SPAM_LOGS
#ifdef ENABLE_SPAM_LOGS
#define SPAM_LOG(level, fmt, ...)                                \
    mc::core::Logger::get()->log(                                \
        spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
        to_spdlog_level(LogLevel::level),                        \
        std::format(fmt __VA_OPT__(, ) __VA_ARGS__))
#else
#define SPAM_LOG(level, fmt, ...) ((void)0)
#endif

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

inline spdlog::level::level_enum to_spdlog_level(LogLevel level)
{
    switch (level)
    {
    case LogLevel::TRACE: return spdlog::level::trace;
    case LogLevel::DEBUG: return spdlog::level::debug;
    case LogLevel::INFO: return spdlog::level::info;
    case LogLevel::WARN: return spdlog::level::warn;
    case LogLevel::ERROR: return spdlog::level::err;
    case LogLevel::CRITICAL: return spdlog::level::critical;
    default: return spdlog::level::debug;
    }
}

namespace mc::core
{
class Logger
{
public:
    static void init();
    static std::shared_ptr<spdlog::logger>& get();

private:
    static std::shared_ptr<spdlog::logger> m_sLogger; ///< Shared logger instance.
};
} // namespace mc::core
