#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace mc::core
{
class Logger
{
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetLogger();

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};
}
