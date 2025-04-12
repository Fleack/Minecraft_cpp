#include "core/Application.hpp"

#include "core/Logger.hpp"

int main()
{
    concurrencpp::runtime_options options;
    options.thread_started_callback = [](std::string_view name) {
        using namespace mc::core;
        LOG(DEBUG, "[START] Thread {} with id={}", name, std::this_thread::get_id());
    };

    options.thread_terminated_callback = [](std::string_view name) {
        using namespace mc::core;
        LOG(DEBUG, "[STOP] Thread {} with id={}", name, std::this_thread::get_id());
    };

    mc::core::Application app{std::move(options)};
    app.run();
    return 0;
}
