#include "core/Application.hpp"
#include "core/CrashReporter.hpp"

#include "core/Logger.hpp"

int main()
{
    mc::core::Logger::init();
    mc::core::CrashReporter::init();
    concurrencpp::runtime_options options;
    options.thread_started_callback = [](std::string_view name) {
        using namespace mc::core;
        LOG(DEBUG, "Starting thread {} with id={}", name, std::this_thread::get_id());
    };

    options.thread_terminated_callback = [](std::string_view name) {
        using namespace mc::core;
        LOG(DEBUG, "Stopping thread {} with id={}", name, std::this_thread::get_id());
    };

    mc::core::Application app{std::move(options)};
    app.run();
    return 0;
}
