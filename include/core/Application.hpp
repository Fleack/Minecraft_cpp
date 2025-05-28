#pragma once

#include "ecs/Ecs.hpp"
#include "ecs/system/CollisionSystem.hpp"
#include "ecs/system/GravitySystem.hpp"
#include "ecs/system/JumpSystem.hpp"
#include "ecs/system/MovementSystem.hpp"
#include "ecs/system/PlayerInputSystem.hpp"
#include "ecs/system/UISystem.hpp"
#include "world/World.hpp"

#include <memory>

#include <Magnum/Platform/Sdl2Application.h>
#include <concurrencpp/concurrencpp.h>

namespace mc::ecs
{
class CameraSystem;
class ChunkLoadingSystem;
class RenderSystem;
} // namespace mc::ecs

namespace mc::core
{
/**
 * @brief Main application class for running the voxel game.
 *
 * Handles initialization, main loop, ECS setup, input, camera, rendering,
 * world management, and shutdown procedures.
 */
class Application final : public Magnum::Platform::Sdl2Application
{
public:
    explicit Application(Arguments const& arguments);

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent& event) override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void pointerMoveEvent(PointerMoveEvent& event) override;
    void pointerPressEvent(PointerEvent& event) override;
    void pointerReleaseEvent(PointerEvent& event) override;
    void scrollEvent(ScrollEvent& event) override;

    void initializeCore() const;

    /**
     * @brief Initializes the ECS (Entity-Component-System) core.
     */
    void initializeEcs();

    void initializePlayer();

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

    void initializeUiSystem();

    /**
     * @brief Cleans up and shuts down the application.
     */
    void shutdown();

private:
    concurrencpp::runtime m_runtime;

    std::shared_ptr<concurrencpp::thread_pool_executor> m_chunkExecutor;
    std::shared_ptr<concurrencpp::thread_pool_executor> m_meshExecutor;
    std::shared_ptr<concurrencpp::manual_executor> m_mainExecutor;

    std::unique_ptr<ecs::Ecs> m_ecs; ///< ECS manager.
    std::unique_ptr<world::World> m_world; ///< Game world and chunk storage.
    std::shared_ptr<ecs::UISystem> m_uiSystem; ///< Handles UI rendering.
    std::shared_ptr<ecs::CameraSystem> m_cameraSystem; ///< Handles camera movement.
    std::shared_ptr<ecs::PlayerInputSystem> m_playerInputSystem;
    std::shared_ptr<ecs::MovementSystem> m_movementSystem;
    std::shared_ptr<ecs::RenderSystem> m_renderSystem; ///< Handles rendering entities.
    std::shared_ptr<ecs::ChunkLoadingSystem> m_chunkLoadingSystem; ///< Dynamically loads chunks.
    std::shared_ptr<ecs::GravitySystem> m_gravitySystem;
    std::shared_ptr<ecs::JumpSystem> m_jumpSystem;
    std::shared_ptr<ecs::CollisionSystem> m_collisionSystem;

    float m_aspectRatio = 1920.0f / 1080.0f;

    bool m_paused{false};
};
} // namespace mc::core
