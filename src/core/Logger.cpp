#include "core/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks-inl.h>

namespace mc::core
{
std::shared_ptr<spdlog::logger> Logger::m_sLogger;

void Logger::init()
{
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    m_sLogger = spdlog::stdout_color_mt("Minecraft");
    m_sLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Logger::get()
{
    return m_sLogger;
}
}
