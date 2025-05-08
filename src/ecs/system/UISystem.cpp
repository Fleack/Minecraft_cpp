#include "ecs/system/UISystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
#include "ecs/component/TransformComponent.hpp"
#include "utils/FastDivFloor.hpp"

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
    : m_ecs{ecs}
    , m_world{world}
    , m_ui{windowSize, Magnum::Ui::McssDarkStyle{}}
    , m_fpsLabel{Corrade::NoCreate, m_ui}
    , m_coordsLabel{Corrade::NoCreate, m_ui}
    , m_chunkLabel{Corrade::NoCreate, m_ui}
    , m_chunksCountLabel{Corrade::NoCreate, m_ui}
    , m_viewLabel{Corrade::NoCreate, m_ui}
    , m_speedLabel{Corrade::NoCreate, m_ui}
    , m_fovLabel{Corrade::NoCreate, m_ui}
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
            {200.0f, 20.0f});
        m_speedLabel = Magnum::Ui::Label{anchor, "Speed: 0.00", textProps};
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

    LOG(INFO, "UISystem initialized");
}

void UISystem::render(float deltaTime)
{
    auto const& transform = m_ecs.getAllComponents<TransformComponent>().begin()->second;
    auto const& cam = m_ecs.getAllComponents<CameraComponent>().begin()->second;

    // FPS
    {
        m_frameTimes.push_back(deltaTime);
        if (m_frameTimes.size() > frameSampleSize)
            m_frameTimes.pop_front();

        m_timeSinceLastFpsUpdate += deltaTime;
        if (m_timeSinceLastFpsUpdate >= 0.5f)
        {
            float fps99 = 1.0f / calculatePercentile(0.99f);
            float fps999 = 1.0f / calculatePercentile(0.999f);
            float avgDeltaTime = std::accumulate(m_frameTimes.begin(), m_frameTimes.end(), 0.0f) / m_frameTimes.size();
            float avgFrameTimeMs = avgDeltaTime * 1000.0f;
            float avgFps = 1.0f / avgDeltaTime;
            float fps = 1.0f / deltaTime;
            m_fpsLabel.setText(
                Corrade::Utility::format("FPS: {} (AVG: {:.0f}, 99%: {:.0f}, 99.9%: {:.0f}, Frametime: {:.1f} ms)", std::lround(fps), avgFps, fps99, fps999, avgFrameTimeMs));
            m_timeSinceLastFpsUpdate = 0.0f;
        }
    }
    // Coords
    {
        auto const& pos = transform.position;
        m_coordsLabel.setText(
            Corrade::Utility::format(
                "Coords: {:.2f}, {:.2f}, {:.2f}",
                pos.x(),
                pos.y(),
                pos.z()));
    }
    // Current chunk
    {
        auto const& pos = transform.position;
        constexpr uint8_t chunkSize = 16;
        int cx = utils::floor_div(pos.x(), chunkSize);
        int cz = utils::floor_div(pos.z(), chunkSize);
        m_chunkLabel.setText(
            Corrade::Utility::format("Chunk: {},{}", cx, cz));
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
            Magnum::Math::cos(Magnum::Rad{cam.pitch}) * Magnum::Math::cos(Magnum::Rad{cam.yaw}),
            Magnum::Math::sin(Magnum::Rad{cam.pitch}),
            Magnum::Math::cos(Magnum::Rad{cam.pitch}) * Magnum::Math::sin(Magnum::Rad{cam.yaw})};

        m_viewLabel.setText(
            Corrade::Utility::format(
                "View: {:.2f}, {:.2f}, {:.2f}",
                front.x(),
                front.y(),
                front.z()));
    }
    // Speed
    {
        auto camSpeed = cam.speed;
        m_speedLabel.setText(
            Corrade::Utility::format("Speed: {:.2f}", camSpeed));
    }
    // FOV
    {
        auto camFov = cam.fov;
        m_fovLabel.setText(
            Corrade::Utility::format("FOV: {:.1f}", camFov));
    }

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

float UISystem::calculatePercentile(float percentile)
{
    if (m_frameTimes.empty()) return 0.0f;
    std::vector<float> sorted(m_frameTimes.begin(), m_frameTimes.end());
    std::sort(sorted.begin(), sorted.end());
    std::size_t index = percentile * (sorted.size() - 1);
    return sorted[index];
}
} // namespace mc::ecs
