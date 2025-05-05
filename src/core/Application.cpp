#include "core/Application.hpp"

#include "core/CrashReporter.hpp"
#include "core/Logger.hpp"
#include "ecs/Ecs.hpp"
#include "ecs/system/CameraSystem.hpp"
#include "ecs/system/ChunkLoadingSystem.hpp"
#include "ecs/system/RenderSystem.hpp"
#include "world/World.hpp"

#include <chrono>

#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Format.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Ui/Anchor.h>
#include <Magnum/Ui/Application.h>
#include <Magnum/Ui/Label.h>
#include <Magnum/Ui/SnapLayouter.h>
#include <Magnum/Ui/Style.h>
#include <Magnum/Ui/UserInterfaceGL.h>

#include "ecs/component/TransformComponent.hpp"

namespace mc::core
{
using namespace Magnum;
using namespace Magnum::Platform;

Application::Application(Arguments const& arguments)
    : Sdl2Application{
          arguments,
          Configuration{}.setTitle("MinecraftX").setSize({1280, 720}),
          GLConfiguration{}.setVersion(GL::Version::GL460)},
      m_ui{windowSize(), Magnum::Ui::McssDarkStyle{}}, m_fpsLabel{Corrade::NoCreate, m_ui}, m_coordsLabel{Corrade::NoCreate, m_ui}, m_chunkLabel{Corrade::NoCreate, m_ui}, m_chunksCountLabel{Corrade::NoCreate, m_ui}, m_chunkExecutor{m_runtime.thread_pool_executor()}, m_mainExecutor{m_runtime.make_manual_executor()}, m_aspectRatio{static_cast<float>(windowSize().x()) / windowSize().y()}
{
    constexpr uint8_t renderDistance = 10;

    initializeCore();
    initializeEcs();
    initializeCamera();
    initializeWorld(renderDistance);
    initializeRenderSystems(renderDistance);

    // FPS
    {
        auto fpsAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            {0.0f, 0.0f},
            {100.0f, 20.0f});
        m_fpsLabel = Magnum::Ui::Label{fpsAnchor, "FPS: 0"};
    }
    // Coords
    {
        auto coordsAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            /* offset */ {0.0f, 20.0f},
            /* size   */ {200.0f, 20.0f});
        m_coordsLabel = Magnum::Ui::Label{coordsAnchor, "Coords: 0, 0, 0"};
    }
    // CurrentChunk
    {
        auto chunkAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            /* offset */ {0.0f, 40.0f},
            /* size   */ {200.0f, 20.0f});
        m_chunkLabel = Magnum::Ui::Label{chunkAnchor, "Chunk: 0,0"};
    }
    // Chunks amount
    {
        auto countAnchor = Magnum::Ui::snap(
            m_ui,
            Magnum::Ui::Snap::TopLeft,
            /* offset */ {0.0f, 60.0f},
            /* size   */ {200.0f, 20.0f});
        m_chunksCountLabel = Magnum::Ui::Label{countAnchor, "Chunks: 0"};
    }

    setCursor(Cursor::Hidden);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::setClearColor(Color4{0.5f, 0.7f, 1.0f, 1.0f});

    setSwapInterval(1);
    redraw();
}

void Application::initializeCore() const
{
    Logger::init();
    CrashReporter::init();
    LOG(INFO, "Core systems initialized");
}

void Application::initializeEcs()
{
    m_ecs = std::make_unique<ecs::Ecs>();
}

void Application::initializeCamera()
{
    m_cameraSystem = std::make_shared<ecs::CameraSystem>(*m_ecs, m_aspectRatio);
    m_ecs->addSystem(m_cameraSystem);
}

void Application::initializeWorld(uint8_t renderDistance)
{
    m_world = std::make_unique<world::World>(m_chunkExecutor, m_mainExecutor);
    m_chunkLoadingSystem = std::make_shared<ecs::ChunkLoadingSystem>(*m_ecs, *m_world, renderDistance);
    m_ecs->addSystem(m_chunkLoadingSystem);
}

void Application::initializeRenderSystems(uint8_t renderDistance)
{
    m_renderSystem = std::make_shared<ecs::RenderSystem>(*m_ecs, m_cameraSystem, *m_world, renderDistance);
    m_ecs->addSystem(m_renderSystem);
}

void Application::drawEvent()
{
    using clock = std::chrono::steady_clock;
    static clock::time_point last = clock::now();
    auto now = clock::now();
    float deltaTime = std::chrono::duration<float>(now - last).count();
    last = now;

    if (auto pending = m_mainExecutor->size(); pending > 0)
        m_mainExecutor->loop(pending);

    GL::defaultFramebuffer.clear(
        GL::FramebufferClear::Color |
        GL::FramebufferClear::Depth);

    if (!m_paused)
    {
        m_ecs->update(deltaTime);
    }
    m_ecs->render();

    auto& component = m_ecs->getAllComponents<mc::ecs::TransformComponent>().begin()->second;
    // FPS
    {
        float const fps = 1.0f / deltaTime;
        m_fpsLabel.setText(
            Corrade::Utility::format("FPS: {}", std::lround(fps)));
    }
    // Coords
    {
        auto const& pos = component.position;
        m_coordsLabel.setText(
            Corrade::Utility::format(
                "Coords: {:.2f}, {:.2f}, {:.2f}",
                pos.x(),
                pos.y(),
                pos.z()));
    }
    // Current chunk
    {
        auto const& pos = component.position;
        constexpr float chunkSize = 16.0f;
        int cx = std::floor(pos.x() / chunkSize);
        int cz = std::floor(pos.z() / chunkSize);
        m_chunkLabel.setText(
            Corrade::Utility::format("Chunk: {},{}", cx, cz));
    }
    // Chunks amount
    {
        std::size_t total = m_world->getChunks().size();
        m_chunksCountLabel.setText(
            Corrade::Utility::format("Chunks: {}", total));
    }

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::One, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    m_ui.draw();
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
    redraw();
}

void Application::viewportEvent(ViewportEvent& event)
{
    m_aspectRatio = static_cast<float>(event.windowSize().x()) / event.windowSize().y();
    m_cameraSystem->setAspectRatio(m_aspectRatio);
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    m_ui.setSize(event.windowSize());
}

void Application::keyPressEvent(KeyEvent& event)
{
    if (event.key() == Key::Esc)
    {
        m_paused = !m_paused;
        setCursor(m_paused ? Cursor::Arrow : Cursor::Hidden);
        SDL_SetRelativeMouseMode(m_paused ? SDL_FALSE : SDL_TRUE);
    }

    if (!m_paused)
    {
        m_cameraSystem->handleKey(event.key(), true);
    }
    event.setAccepted();
}

void Application::keyReleaseEvent(KeyEvent& event)
{
    if (!m_paused)
    {
        m_cameraSystem->handleKey(event.key(), false);
    }
    event.setAccepted();
}

void Application::pointerMoveEvent(Magnum::Platform::Sdl2Application::PointerMoveEvent& event)
{
    if (m_paused)
    {
        m_ui.pointerMoveEvent(event);
    }
    else
    {
        m_cameraSystem->handleMouse(event.relativePosition());
        event.setAccepted();
    }
}

void Application::pointerPressEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    if (m_paused && m_ui.pointerPressEvent(event)) return;
}

void Application::pointerReleaseEvent(Magnum::Platform::Sdl2Application::PointerEvent& event)
{
    if (m_paused && m_ui.pointerReleaseEvent(event)) return;
}

void Application::scrollEvent(ScrollEvent& event)
{
    if (!m_paused)
    {
        m_cameraSystem->handleScroll(event.offset().y());
    }
    event.setAccepted();
}

void Application::shutdown()
{
    m_renderSystem.reset();
    m_cameraSystem.reset();
    m_chunkLoadingSystem.reset();
    m_world.reset();
    m_ecs.reset();
}

} // namespace mc::core
