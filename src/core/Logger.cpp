#include "core/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks-inl.h>

namespace mc::core
{
std::shared_ptr<spdlog::logger> Logger::s_logger;

void Logger::init()
{
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    s_logger = spdlog::stdout_color_mt("Minecraft");
    s_logger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Logger::get()
{
    return s_logger;
}
}
