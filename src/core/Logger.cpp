#include "core/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks-inl.h>

namespace mc::core
{
std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    s_Logger = spdlog::stdout_color_mt("Minecraft");
    s_Logger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Logger::GetLogger()
{
    return s_Logger;
}
}
