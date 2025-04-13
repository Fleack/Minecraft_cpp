#include "core/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks-inl.h>

namespace mc::core
{
std::shared_ptr<spdlog::logger> Logger::m_sLogger;

void Logger::init()
{
    spdlog::init_thread_pool(8192, 1);

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    m_sLogger = std::make_shared<spdlog::async_logger>(
        "Minecraft",
        consoleSink,
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block
    );

    spdlog::register_logger(m_sLogger);

    spdlog::set_default_logger(m_sLogger);
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Logger::get()
{
    return m_sLogger;
}
}
