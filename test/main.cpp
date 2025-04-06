#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "core/Logger.hpp"

#include "TestEventListener.hpp"
CATCH_REGISTER_LISTENER(TestLoggerListener)

int main(int argc, char* argv[])
{
    mc::core::Logger::init();
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Running unit tests...");
    return Catch::Session().run(argc, argv);
}
