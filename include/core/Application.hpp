#pragma once

#include <memory>

#include <concurrencpp/concurrencpp.h>

namespace mc::render
{
class TextureAtlas;
}

namespace mc::input
{
class IInputProvider;
}

namespace mc::world
{
class World;
}

namespace mc::ecs
{
class ChunkLoadingSystem;
class RenderSystem;
class CameraSystem;
class Ecs;
class ISystem;
} // namespace mc::ecs

namespace mc::core
{
class Window;

/**
 * @brief Main application class for running the voxel game.
 *
 * Handles initialization, main loop, ECS setup, input, camera, rendering,
 * world management, and shutdown procedures.
 */
class Application
{
public:
    Application(concurrencpp::runtime_options&& options);
    ~Application();

    /**
     * @brief Runs the main game loop.
     *
     * Handles per-frame update and rendering.
     */
    void run();

private:
    /**
     * @brief Performs all necessary initialization steps.
     *
     * @return True if initialization succeeded, false otherwise.
     */
    bool initialize();

    /**
     * @brief Initializes the main application window.
     *
     * @return True if the window was created successfully.
     */
    bool initializeWindow();

    /**
     * @brief Initializes the ECS (Entity-Component-System) core.
     */
    void initializeEcs();

    /**
     * @brief Initializes input handling and maps it to the ECS.
     */
    void initializeInput();

    /**
     * @brief Sets up the camera system and default camera entity.
     */
    void initializeCamera();

    /**
     * @brief Initializes the world and loads the initial chunks.
     *
     * @param renderDistance render distance from camera position
     */
    void initializeWorld(uint8_t renderDistance);

    /**
     * @brief Registers and sets up render-related ECS systems.
     *
     * @param renderDistance render distance from camera position
     */
    void initializeRenderSystems(uint8_t renderDistance);

    /**
     * @brief Updates game logic and ECS systems.
     *
     * @param deltaTime Time since the last frame in seconds.
     */
    void update(float deltaTime);

    /**
     * @brief Renders the current frame.
     */
    void render();

    /**
     * @brief Cleans up and shuts down the application.
     */
    void shutdown();

private:
    concurrencpp::runtime m_runtime;
    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor;

    std::unique_ptr<Window> m_window; ///< Window context.
    std::unique_ptr<ecs::Ecs> m_ecs; ///< ECS manager.
    std::unique_ptr<world::World> m_world; ///< Game world and chunk storage.
    std::shared_ptr<input::IInputProvider> m_inputProvider; ///< Input abstraction layer.
    std::shared_ptr<ecs::CameraSystem> m_cameraSystem; ///< Handles camera movement.
    std::shared_ptr<ecs::RenderSystem> m_renderSystem; ///< Handles rendering entities.
    std::shared_ptr<ecs::ChunkLoadingSystem> m_chunkLoadingSystem; ///< Dynamically loads chunks.

    double m_lastFrameTime = 0.0; ///< Time of the previous frame (for delta time calculation).
};
} // namespace mc::core
