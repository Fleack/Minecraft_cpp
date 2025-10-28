#pragma once

#include "ecs/Ecs.hpp"
#include "world/SimpleChunkProvider.hpp"

#include <memory>

#include <Magnum/Platform/Sdl2Application.h>
#include <concurrencpp/concurrencpp.h>

namespace mc::world
{
class SimpleChunkProvider;
}

namespace mc::ecs
{
class CameraSystem;
class RenderSystem;
class UISystem;
class PlayerInputSystem;
} // namespace mc::ecs

namespace mc::core
{
class ClientApplication final : public Magnum::Platform::Sdl2Application
{
public:
    explicit ClientApplication(Arguments const& arguments);

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent& event) override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void pointerMoveEvent(PointerMoveEvent& event) override;
    void pointerPressEvent(PointerEvent& event) override;
    void pointerReleaseEvent(PointerEvent& event) override;
    void scrollEvent(ScrollEvent& event) override;

    void initializeSystems();

    void shutdown();

private:
    concurrencpp::runtime m_runtime;

    std::shared_ptr<concurrencpp::thread_pool_executor> m_meshExecutor;
    std::shared_ptr<concurrencpp::manual_executor> m_mainExecutor;

    std::unique_ptr<ecs::Ecs> m_ecs; ///< ECS manager.
    std::unique_ptr<world::SimpleChunkProvider> m_chunkProvider; ///< Provides chunks for rendering.

    // TODO: Create storage for systems
    std::shared_ptr<ecs::UISystem> m_uiSystem; ///< Handles UI rendering.
    std::shared_ptr<ecs::CameraSystem> m_cameraSystem; ///< Handles camera movement.
    std::shared_ptr<ecs::PlayerInputSystem> m_playerInputSystem; ///< Handles player input.
    std::shared_ptr<ecs::RenderSystem> m_renderSystem; ///< Handles rendering entities.

    float m_aspectRatio{1920.0f / 1080.0f};
    static constexpr uint8_t RENDER_DISTANCE{10};

    bool m_paused{false};
};
} // namespace mc::core
