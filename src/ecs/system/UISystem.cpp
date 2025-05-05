#include "ecs/system/UISystem.hpp"

#include "core/Logger.hpp"
#include "ecs/component/CameraComponent.hpp"
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
        auto fpsAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 0.0f},
            {150.0f, 20.0f});
        m_fpsLabel = Magnum::Ui::Label{fpsAnchor, "FPS: 0", textProps};
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
        float const fps = 1.0f / deltaTime;
        m_fpsLabel.setText(
            Corrade::Utility::format("FPS: {}", std::lround(fps)));
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
        constexpr float chunkSize = 16.0f;
        int cx = std::floor(pos.x() / chunkSize);
        int cz = std::floor(pos.z() / chunkSize);
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
} // namespace mc::ecs
