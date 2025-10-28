#include "systems/UISystem.hpp"

#include "ecs/component/CameraComponent.hpp"
#include "world/Chunk.hpp"

#include <Corrade/Utility/Format.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Text/Alignment.h>
#include <Magnum/Ui/Anchor.h>
#include <Magnum/Ui/Application.h>
#include <Magnum/Ui/Event.h>
#include <Magnum/Ui/Label.h>
#include <Magnum/Ui/SnapLayouter.h>
#include <Magnum/Ui/Style.h>
#include <Magnum/Ui/TextProperties.h>
#include <core/Logger.hpp>
#include <ecs/component/PlayerComponent.hpp>
#include <ecs/component/TransformComponent.hpp>

namespace mc::ecs
{
UISystem::UISystem(Ecs& ecs, Magnum::Vector2i windowSize)
    : m_ecs{ecs}
    , m_ui{windowSize, Magnum::Ui::McssDarkStyle{}}
    , m_transformComponent{m_ecs.getAllComponents<TransformComponent>().begin()->second}
    , m_cameraComponent{m_ecs.getAllComponents<CameraComponent>().begin()->second}
    , m_playerComponent{m_ecs.getAllComponents<PlayerComponent>().begin()->second}
    , m_velocityComponent{m_ecs.getAllComponents<VelocityComponent>().begin()->second}
{
    using namespace Magnum::Math::Literals;

    Magnum::Ui::TextProperties textProps;
    textProps.setAlignment(Magnum::Text::Alignment::BottomLeft);

    std::vector<LabelDef> labels = {
        {"FPS: 0 (AVG: 0, 99%: 0, 99.9%: 0, Frametime: 0 ms)", {0, 0}, 400, &m_fpsLabel},
        {"Coords: 0, 0, 0", {0, 20}, 200, &m_coordsLabel},
        {"Chunk: 0,0", {0, 40}, 200, &m_chunkLabel},
        {"Chunks: 0", {0, 60}, 200, &m_chunksCountLabel},
        {"View: 0.00, 0.00, 0.00", {0, 80}, 200, &m_viewLabel},
        {"Velocity: 0.0 [X: 0.0, Y: 0.0, Z: 0.0]: Max Speed: 0.00", {0, 100}, 300, &m_speedLabel},
        {"FOV: 0.00", {0, 120}, 200, &m_fovLabel},
        {"onGround: false", {0, 140}, 200, &m_playerInfo},
    };

    for (auto& label : labels)
    {
        auto anchor = Magnum::Ui::snap(m_ui, Magnum::Ui::Snap::TopLeft, label.offset, {label.width, 20});
        *label.label = Magnum::Ui::Label{anchor, label.name + ":", textProps};
    }

    LOG(INFO, "UISystem initialized");
}

void UISystem::render(float deltaTime)
{
    renderWithoutInterval();
    renderWithInterval(deltaTime);

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::One, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    m_ui.draw();
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
}

bool UISystem::pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent& event)
{
    return m_ui.pointerMoveEvent(event);
}

bool UISystem::pointerPressEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    return m_ui.pointerPressEvent(event);
}

bool UISystem::pointerReleaseEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    return m_ui.pointerReleaseEvent(event);
}

void UISystem::setWindowSize(Magnum::Vector2i windowSize)
{
    m_ui.setSize(windowSize);
}

void UISystem::renderWithoutInterval()
{
    // Coords
    {
        auto const& pos = m_transformComponent.position;
        m_coordsLabel.setText(
            Corrade::Utility::format(
                "Coords: {:.2f}, {:.2f}, {:.2f}",
                pos.x(),
                pos.y(),
                pos.z()));
    }
    // Current chunk
    {
        auto const& pos = m_transformComponent.position;
        auto chunkPos = world::Chunk::getChunkOfPosition(pos);
        m_chunkLabel.setText(
            Corrade::Utility::format("Chunk: {},{}", chunkPos.x(), chunkPos.z()));
    }
    // Chunks amount
    {
        std::size_t totalChunks = 0; // TODO: get from server
        m_chunksCountLabel.setText(
            Corrade::Utility::format("Chunks: {}", totalChunks));
    }
    // Camera view
    {
        Magnum::Vector3 front{
            Magnum::Math::cos(Magnum::Rad{m_cameraComponent.pitch}) * Magnum::Math::cos(Magnum::Rad{m_cameraComponent.yaw}),
            Magnum::Math::sin(Magnum::Rad{m_cameraComponent.pitch}),
            Magnum::Math::cos(Magnum::Rad{m_cameraComponent.pitch}) * Magnum::Math::sin(Magnum::Rad{m_cameraComponent.yaw})};

        m_viewLabel.setText(
            Corrade::Utility::format(
                "View: {:.2f}, {:.2f}, {:.2f}",
                front.x(),
                front.y(),
                front.z()));
    }
    // Velocity
    {
        auto maxSpeed = m_velocityComponent.maxSpeed;
        auto velocity = m_velocityComponent.velocity.length() * 2;
        auto velocityX = m_velocityComponent.velocity.x();
        auto velocityY = m_velocityComponent.velocity.y();
        auto velocityZ = m_velocityComponent.velocity.z();
        m_speedLabel.setText(
            Corrade::Utility::format("Velocity: {:.1f} [X: {:.1f}, Y: {:.1f}, Z: {:.1f}]: Max Speed: {:.2f}", velocity, velocityX, velocityY, velocityZ, maxSpeed));
    }
    // FOV
    {
        auto camFov = m_cameraComponent.fov;
        m_fovLabel.setText(
            Corrade::Utility::format("FOV: {:.1f}", camFov));
    }
    // Player Component
    {
        auto onGround = m_playerComponent.onGround ? "true" : "false";
        m_playerInfo.setText(
            Corrade::Utility::format("onGround: {}", onGround));
    }
}

void UISystem::renderWithInterval(float deltaTime)
{
    if (m_timeSinceLastFpsUpdate <= FPS_UPDATE_INTERVAL)
    {
        m_timeSinceLastFpsUpdate += deltaTime;
        return;
    }
    m_timeSinceLastFpsUpdate = 0.0f;

    // FPS
    {
        m_fpsStats.pushFrame(deltaTime);
        m_fpsLabel.setText(
            Corrade::Utility::format(
                "FPS: {:.0f} (AVG: {:.0f}, 99%: {:.0f}, 99.9%: {:.0f}, Frametime: {:.1f} ms)",
                m_fpsStats.currentFps(),
                m_fpsStats.averageFps(),
                m_fpsStats.fps99(),
                m_fpsStats.fps999(),
                deltaTime * 1000.0f));
    }
}
} // namespace mc::ecs
