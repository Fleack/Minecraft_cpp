#include "core/Logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks-inl.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mc::core
{
std::shared_ptr<spdlog::logger> Logger::m_sLogger;

void Logger::init()
{
    spdlog::init_thread_pool(8192, 1);

#define CONSOLE_LOG
#ifdef CONSOLE_LOG
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::trace);
#endif

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Last.log", true);
    fileSink->set_level(spdlog::level::trace);

#ifdef CONSOLE_LOG
    std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
#else
    std::vector<spdlog::sink_ptr> sinks{fileSink};
#endif

    m_sLogger = std::make_shared<spdlog::async_logger>(
        "Minecraft",
        sinks.begin(),
        sinks.end(),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);

    spdlog::register_logger(m_sLogger);
    spdlog::set_default_logger(m_sLogger);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v (%s:%#)");
    spdlog::set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Logger::get()
{
    return m_sLogger;
}
} // namespace mc::core
