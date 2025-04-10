#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace mc::core
{
/**
 * @brief Singleton-style wrapper for the global spdlog logger.
 *
 * Provides a simple interface to initialize and access the core logger
 * throughout the application.
 */
class Logger
{
public:
    /**
     * @brief Initializes the global logger instance.
     *
     * Sets up formatting, log level, and output destinations.
     * Should be called once at application startup.
     */
    static void init();

    /**
     * @brief Returns a reference to the shared logger instance.
     *
     * @return Reference to the spdlog logger.
     */
    static std::shared_ptr<spdlog::logger>& get();

private:
    static std::shared_ptr<spdlog::logger> m_sLogger; ///< Shared logger instance.
};
}
