#include "ecs/system/UISystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/PlayerComponent.hpp"
#include "ecs/component/TransformComponent.hpp"

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

namespace mc::ecs
{
UISystem::UISystem(Ecs& ecs, world::World const& world, Magnum::Vector2i windowSize)
    : ISystem{Type::UI}
    , m_ecs{ecs}
    , m_world{world}
    , m_ui{windowSize, Magnum::Ui::McssDarkStyle{}}
    , m_fpsLabel{Corrade::NoCreate, m_ui}
    , m_coordsLabel{Corrade::NoCreate, m_ui}
    , m_chunkLabel{Corrade::NoCreate, m_ui}
    , m_chunksCountLabel{Corrade::NoCreate, m_ui}
    , m_viewLabel{Corrade::NoCreate, m_ui}
    , m_speedLabel{Corrade::NoCreate, m_ui}
    , m_fovLabel{Corrade::NoCreate, m_ui}
    , m_playerInfo{Corrade::NoCreate, m_ui}
    , m_transformComponent{m_ecs.getAllComponents<TransformComponent>().begin()->second}
    , m_cameraComponent{m_ecs.getAllComponents<CameraComponent>().begin()->second}
    , m_playerComponent{m_ecs.getAllComponents<PlayerComponent>().begin()->second}
    , m_velocityComponent{m_ecs.getAllComponents<VelocityComponent>().begin()->second}
{
    using namespace Magnum::Math::Literals;

    Magnum::Ui::TextProperties textProps;
    textProps.setAlignment(Magnum::Text::Alignment::BottomLeft);
    // FPS
    {
        m_frameTimes.resize(frameSampleSize);
        auto fpsAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 0.0f},
            {400.0f, 20.0f});
        m_fpsLabel = Magnum::Ui::Label{fpsAnchor, "FPS: 0 (AVG: 0, 99%: 0, 99.9%: 0, Frametime: 0 ms)", textProps};
    }
    // Coords
    {
        auto coordsAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 20.0f},
            {200.0f, 20.0f});
        m_coordsLabel = Magnum::Ui::Label{coordsAnchor, "Coords: 0, 0, 0", textProps};
    }
    // CurrentChunk
    {
        auto chunkAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 40.0f},
            {200.0f, 20.0f});
        m_chunkLabel = Magnum::Ui::Label{chunkAnchor, "Chunk: 0,0", textProps};
    }
    // Chunks amount
    {
        auto countAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 60.0f},
            {200.0f, 20.0f});
        m_chunksCountLabel = Magnum::Ui::Label{countAnchor, "Chunks: 0", textProps};
    }
    // Camera view
    {
        auto viewAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 80.0f},
            {200.0f, 20.0f});
        m_viewLabel = Magnum::Ui::Label{viewAnchor, "View: 0.00, 0.00, 0.00", textProps};
    }
    // Speed
    {
        auto anchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 100.0f},
            {250.0f, 20.0f});
        m_speedLabel = Magnum::Ui::Label{anchor, "Velocity: [X: 0.0, Y: 0.0, Z: 0.0], Speed: 0.00", textProps};
    }
    // FOV
    {
        auto anchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 120.0f},
            {200.0f, 20.0f});
        m_fovLabel = Magnum::Ui::Label{anchor, "FOV: 0.00", textProps};
    }
    // Player Component
    {
        auto anchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 140.0f},
            {200.0f, 20.0f});
        m_playerInfo = Magnum::Ui::Label{anchor, "onGround: false", textProps};
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
        auto chunkPos = world::World::getChunkOfPosition(static_cast<Magnum::Vector3i>(pos));
        m_chunkLabel.setText(
            Corrade::Utility::format("Chunk: {},{}", chunkPos.x(), chunkPos.z()));
    }
    // Chunks amount
    {
        std::size_t totalChunks = m_world.getChunks().size();
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
        auto camSpeed = m_velocityComponent.speed;
        auto velocityX = m_velocityComponent.velocity.x();
        auto velocityY = m_velocityComponent.velocity.y();
        auto velocityZ = m_velocityComponent.velocity.z();
        m_speedLabel.setText(
            Corrade::Utility::format("Velocity: [X: {:.1f}, Y: {:.1f}, Z: {:.1f}], Speed: {:.2f}", velocityX, velocityY, velocityZ, camSpeed));
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
    if (m_timeSinceLastFpsUpdate <= fpsUpdateInterval)
    {
        m_timeSinceLastFpsUpdate += deltaTime;
        return;
    }
    m_timeSinceLastFpsUpdate = 0.0f;

    // FPS
    {
        m_frameTimes.push_back(deltaTime);
        if (m_frameTimes.size() > frameSampleSize)
            m_frameTimes.pop_front();

        float fps99 = 1.0f / calculatePercentile(0.99f);
        float fps999 = 1.0f / calculatePercentile(0.999f);
        float avgDeltaTime = std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0f) / m_frameTimes.size();
        float avgFrameTimeMs = avgDeltaTime * 1000.0f;
        float avgFps = 1.0f / avgDeltaTime;
        float fps = 1.0f / deltaTime;
        m_fpsLabel.setText(
            Corrade::Utility::format("FPS: {} (AVG: {:.0f}, 99%: {:.0f}, 99.9%: {:.0f}, Frametime: {:.1f} ms)", std::lround(fps), avgFps, fps99, fps999, avgFrameTimeMs));
    }
}

float UISystem::calculatePercentile(float percentile)
{
    if (m_frameTimes.empty()) return 0.0f;
    std::vector<float> sorted(m_frameTimes.begin(), m_frameTimes.end());
    std::sort(sorted.begin(), sorted.end());
    std::size_t index = percentile * (sorted.size() - 1);
    return sorted[index];
}
} // namespace mc::ecs
