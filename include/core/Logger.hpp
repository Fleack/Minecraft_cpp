#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace mc::core
{
class Logger
{
public:
    static void init();
    static std::shared_ptr<spdlog::logger>& get();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};
}
